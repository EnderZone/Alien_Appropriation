#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>

#include "resource_manager.h"

namespace game {

ResourceManager::ResourceManager(void){
}


ResourceManager::~ResourceManager(){
}


void ResourceManager::AddResource(ResourceType type, const std::string name, GLuint resource, GLsizei size){

    Resource *res;

    res = new Resource(type, name, resource, size);

    mResource.push_back(res);
}


void ResourceManager::AddResource(ResourceType type, const std::string name, GLuint array_buffer, GLuint element_array_buffer, GLsizei size){

    Resource *res;

    res = new Resource(type, name, array_buffer, element_array_buffer, size);

    mResource.push_back(res);
}


void ResourceManager::LoadResource(ResourceType type, const std::string name, const char *filename){

    // Call appropriate method depending on type of resource
    if (type == Material){
        LoadMaterial(name, filename);
    } else {
        throw(std::invalid_argument(std::string("Invalid type of resource")));
    }
}


Resource *ResourceManager::GetResource(const std::string name) const {

    // Find resource with the specified name
    for (int i = 0; i < mResource.size(); i++){
        if (mResource[i]->GetName() == name){
            return mResource[i];
        }
    }
    return NULL;
}


void ResourceManager::LoadMaterial(const std::string name, const char *prefix){

    // Load vertex program source code
    std::string filename = std::string(prefix) + std::string(VERTEX_PROGRAM_EXTENSION);
    std::string vp = LoadTextFile(filename.c_str());

    // Load fragment program source code
    filename = std::string(prefix) + std::string(FRAGMENT_PROGRAM_EXTENSION);
    std::string fp = LoadTextFile(filename.c_str());

    // Create a shader from the vertex program source code
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const char *source_vp = vp.c_str();
    glShaderSource(vs, 1, &source_vp, NULL);
    glCompileShader(vs);

    // Check if shader compiled successfully
    GLint status;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE){
        char buffer[512];
        glGetShaderInfoLog(vs, 512, NULL, buffer);
        throw(std::ios_base::failure(std::string("Error compiling vertex shader: ")+std::string(buffer)));
    }

    // Create a shader from the fragment program source code
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    const char *source_fp = fp.c_str();
    glShaderSource(fs, 1, &source_fp, NULL);
    glCompileShader(fs);

    // Check if shader compiled successfully
    glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE){
        char buffer[512];
        glGetShaderInfoLog(fs, 512, NULL, buffer);
        throw(std::ios_base::failure(std::string("Error compiling fragment shader: ")+std::string(buffer)));
    }

    // Create a shader program linking both vertex and fragment shaders
    // together
    GLuint sp = glCreateProgram();
    glAttachShader(sp, vs);
    glAttachShader(sp, fs);
    glLinkProgram(sp);

    // Check if shaders were linked successfully
    glGetProgramiv(sp, GL_LINK_STATUS, &status);
    if (status != GL_TRUE){
        char buffer[512];
        glGetShaderInfoLog(sp, 512, NULL, buffer);
        throw(std::ios_base::failure(std::string("Error linking shaders: ")+std::string(buffer)));
    }

    // Delete memory used by shaders, since they were already compiled
    // and linked
    glDeleteShader(vs);
    glDeleteShader(fs);

    // Add a resource for the shader program
    AddResource(Material, name, sp, 0);
}


std::string ResourceManager::LoadTextFile(const char *filename){

    // Open file
    std::ifstream f;
    f.open(filename);
    if (f.fail()){
        throw(std::ios_base::failure(std::string("Error opening file ")+std::string(filename)));
    }

    // Read file
    std::string content;
    std::string line;
    while(std::getline(f, line)){
        content += line + "\n";
    }

    // Close file
    f.close();

    return content;
}


void ResourceManager::CreateTorus(std::string object_name, float loop_radius, float circle_radius, int num_loop_samples, int num_circle_samples){

    // Create a torus
    // The torus is built from a large loop with small circles around the loop

    // Number of vertices and faces to be created
    // Check the construction algorithm below to understand the numbers
    // specified below
    const GLuint vertex_num = num_loop_samples*num_circle_samples;
    const GLuint face_num = num_loop_samples*num_circle_samples*2;

    // Number of attributes for vertices and faces
    const int vertex_att = 11;
    const int face_att = 3;

    // Data buffers for the torus
    GLfloat *vertex = NULL;
    GLuint *face = NULL;

    // Allocate memory for buffers
    try {
        vertex = new GLfloat[vertex_num * vertex_att]; // 11 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3), 2D texture coordinates (2)
        face = new GLuint[face_num * face_att]; // 3 indices per face
    }
    catch  (std::exception &e){
        throw e;
    }

    // Create vertices 
    float theta, phi; // Angles for circles
    glm::vec3 loop_center;
    glm::vec3 vertex_position;
    glm::vec3 vertex_normal;
    glm::vec3 vertex_color;
    glm::vec2 vertex_coord;

    for (int i = 0; i < num_loop_samples; i++){ // large loop
        
        theta = 2.0*glm::pi<GLfloat>()*i/num_loop_samples; // loop sample (angle theta)
        loop_center = glm::vec3(loop_radius*cos(theta), loop_radius*sin(theta), 0); // centre of a small circle

        for (int j = 0; j < num_circle_samples; j++){ // small circle
            
            phi = 2.0*glm::pi<GLfloat>()*j/num_circle_samples; // circle sample (angle phi)
            
            // Define position, normal and color of vertex
            vertex_normal = glm::vec3(cos(theta)*cos(phi), sin(theta)*cos(phi), sin(phi));
            vertex_position = loop_center + vertex_normal*circle_radius;
            vertex_color = glm::vec3(1.0 - ((float) i / (float) num_loop_samples), 
                                            (float) i / (float) num_loop_samples, 
                                            (float) j / (float) num_circle_samples);
            vertex_coord = glm::vec2(theta / (2.0*glm::pi<GLfloat>()),
                                     phi / (2.0*glm::pi<GLfloat>()));

            // Add vectors to the data buffer
            for (int k = 0; k < 3; k++){
                vertex[(i*num_circle_samples+j)*vertex_att + k] = vertex_position[k];
                vertex[(i*num_circle_samples+j)*vertex_att + k + 3] = vertex_normal[k];
                vertex[(i*num_circle_samples+j)*vertex_att + k + 6] = vertex_color[k];
            }
            vertex[(i*num_circle_samples+j)*vertex_att + 9] = vertex_coord[0];
            vertex[(i*num_circle_samples+j)*vertex_att + 10] = vertex_coord[1];
        }
    }

    // Create triangles
    for (int i = 0; i < num_loop_samples; i++){
        for (int j = 0; j < num_circle_samples; j++){
            // Two triangles per quad
            glm::vec3 t1(((i + 1) % num_loop_samples)*num_circle_samples + j, 
                         i*num_circle_samples + ((j + 1) % num_circle_samples),
                         i*num_circle_samples + j);    
            glm::vec3 t2(((i + 1) % num_loop_samples)*num_circle_samples + j,
                         ((i + 1) % num_loop_samples)*num_circle_samples + ((j + 1) % num_circle_samples),
                         i*num_circle_samples + ((j + 1) % num_circle_samples));
            // Add two triangles to the data buffer
            for (int k = 0; k < 3; k++){
                face[(i*num_circle_samples+j)*face_att*2 + k] = (GLuint) t1[k];
                face[(i*num_circle_samples+j)*face_att*2 + k + face_att] = (GLuint) t2[k];
            }
        }
    }

    // Create OpenGL buffers and copy data
    //GLuint vao;
    //glGenVertexArrays(1, &vao);
    //glBindVertexArray(vao);

    GLuint vbo, ebo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_num * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_num * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);

    // Free data buffers
    delete [] vertex;
    delete [] face;

    // Create resource
    AddResource(Mesh, object_name, vbo, ebo, face_num * face_att);
}


void ResourceManager::CreateSphere(std::string object_name, float radius, int num_samples_theta, int num_samples_phi){

    // Create a sphere using a well-known parameterization

    // Number of vertices and faces to be created
    const GLuint vertex_num = num_samples_theta*num_samples_phi;
    const GLuint face_num = num_samples_theta*(num_samples_phi-1)*2;

    // Number of attributes for vertices and faces
    const int vertex_att = 11;
    const int face_att = 3;

    // Data buffers 
    GLfloat *vertex = NULL;
    GLuint *face = NULL;

    // Allocate memory for buffers
    try {
        vertex = new GLfloat[vertex_num * vertex_att]; // 11 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3), 2D texture coordinates (2)
        face = new GLuint[face_num * face_att]; // 3 indices per face
    }
    catch  (std::exception &e){
        throw e;
    }

    // Create vertices 
    float theta, phi; // Angles for parametric equation
    glm::vec3 vertex_position;
    glm::vec3 vertex_normal;
    glm::vec3 vertex_color;
    glm::vec2 vertex_coord;
   
    for (int i = 0; i < num_samples_theta; i++){
            
        theta = 2.0*glm::pi<GLfloat>()*i/(num_samples_theta-1); // angle theta
            
        for (int j = 0; j < num_samples_phi; j++){
                    
            phi = glm::pi<GLfloat>()*j/(num_samples_phi-1); // angle phi

            // Define position, normal and color of vertex
            vertex_normal = glm::vec3(cos(theta)*sin(phi), sin(theta)*sin(phi), -cos(phi));
            // We need z = -cos(phi) to make sure that the z coordinate runs from -1 to 1 as phi runs from 0 to pi
            // Otherwise, the normal will be inverted
            vertex_position = glm::vec3(vertex_normal.x*radius, 
                                        vertex_normal.y*radius, 
                                        vertex_normal.z*radius),
            vertex_color = glm::vec3(((float)i)/((float)num_samples_theta), 1.0-((float)j)/((float)num_samples_phi), ((float)j)/((float)num_samples_phi));
            vertex_coord = glm::vec2(((float)i)/((float)num_samples_theta), 1.0-((float)j)/((float)num_samples_phi));

            // Add vectors to the data buffer
            for (int k = 0; k < 3; k++){
                vertex[(i*num_samples_phi+j)*vertex_att + k] = vertex_position[k];
                vertex[(i*num_samples_phi+j)*vertex_att + k + 3] = vertex_normal[k];
                vertex[(i*num_samples_phi+j)*vertex_att + k + 6] = vertex_color[k];
            }
            vertex[(i*num_samples_phi+j)*vertex_att + 9] = vertex_coord[0];
            vertex[(i*num_samples_phi+j)*vertex_att + 10] = vertex_coord[1];
        }
    }

    // Create faces
    for (int i = 0; i < num_samples_theta; i++){
        for (int j = 0; j < (num_samples_phi-1); j++){
            // Two triangles per quad
            glm::vec3 t1(((i + 1) % num_samples_theta)*num_samples_phi + j, 
                         i*num_samples_phi + (j + 1),
                         i*num_samples_phi + j);
            glm::vec3 t2(((i + 1) % num_samples_theta)*num_samples_phi + j, 
                         ((i + 1) % num_samples_theta)*num_samples_phi + (j + 1), 
                         i*num_samples_phi + (j + 1));
            // Add two triangles to the data buffer
            for (int k = 0; k < 3; k++){
                face[(i*(num_samples_phi-1)+j)*face_att*2 + k] = (GLuint) t1[k];
                face[(i*(num_samples_phi-1)+j)*face_att*2 + k + face_att] = (GLuint) t2[k];
            }
        }
    }

    // Create OpenGL buffers and copy data
    //GLuint vao;
    //glGenVertexArrays(1, &vao);
    //glBindVertexArray(vao);

    GLuint vbo, ebo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_num * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_num * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);

    // Free data buffers
    delete [] vertex;
    delete [] face;

    // Create resource
    AddResource(Mesh, object_name, vbo, ebo, face_num * face_att);
}

void ResourceManager::CreateCylinder(std::string object_name, float height /*= 1.0*/, float circle_radius /*= 0.5*/, int num_circle_samples /*= 30*/)
{
	// This is a cylinder, 
	// created by making 2 parallel circles a distance of 'height' apart, 
	// then connecting them through a series of faces

	// Number of vertices and faces to be created
	const GLuint vertex_num = 2 * num_circle_samples;
	const GLuint face_num = (num_circle_samples) * 2 + num_circle_samples * 2;

	// Number of attributes for vertices and faces
	const int vertex_att = 11; 
	const int face_att = 3; 

	// Data buffers for the object
	GLfloat *vertex = nullptr;
	GLuint *face = nullptr;

	// Allocate memory for buffers
	try {
		vertex = new GLfloat[vertex_num * vertex_att];  // 11 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3), 2D texture coordinates (2)
		face = new GLuint[face_num * face_att]; // Vertex indices (3)
	}
	catch (std::exception &e) {
		throw e;
	}

	// Create vertices 
	float theta; // Angle for circles
	glm::vec3 circle_center = glm::vec3(0, 0, 0);

	glm::vec3 vertex_position;
	glm::vec3 vertex_normal;
	glm::vec3 vertex_color;
	glm::vec2 vertex_coord;

	// first circle
	for (int i = 0; i < num_circle_samples; i++) {

		theta = 2.0f * glm::pi<GLfloat>()*i / num_circle_samples; // loop sample (angle theta)

		// Define position, normal and color of vertex
		vertex_normal = glm::vec3(0.0, 1.0, 0.0);
		vertex_position = circle_center + glm::vec3(sin(theta) *circle_radius, height / 2.0, cos(theta) *circle_radius);
		vertex_color = glm::vec3(1.0 - ((float)i / (float)num_circle_samples),
			(float)i / (float)num_circle_samples,
			(float)i / (float)num_circle_samples);
		vertex_coord = glm::vec2(1.0 - ((float)i / (float)num_circle_samples), (float)i / (float)num_circle_samples);

		// Add vectors to the data buffer
		for (int k = 0; k < 3; k++) {
			vertex[i*vertex_att + k] = vertex_position[k];
			vertex[i*vertex_att + k + 3] = vertex_normal[k];
			vertex[i*vertex_att + k + 6] = vertex_color[k];

		}
		vertex[i*vertex_att + 9] = vertex_coord[0];
		vertex[i*vertex_att + 10] = vertex_coord[1];
	}

	// Create triangles for first circle
	for (int i = 1; i < num_circle_samples; i++) {

		//triangle from first vertex to current vertex and the next one
		glm::vec3 t(0, i, (i + 1) % num_circle_samples);

		// Add triangle to the data buffer
		for (int k = 0; k < 3; k++)
		{
			face[(i)*face_att + k] = (GLuint)t[k];
		}
	}

	// second circle
	for (int i = 0; i < num_circle_samples; i++) {

		theta = 2.0f * glm::pi<GLfloat>()*i / num_circle_samples; // loop sample (angle theta)

		// Define position, normal and color of vertex
		vertex_normal = glm::vec3(0.0, -1.0, 0.0);
		vertex_position = circle_center + glm::vec3(sin(theta) *circle_radius, -height / 2.0, cos(theta) *circle_radius);
		vertex_color = glm::vec3(1.0 - ((float)i / (float)num_circle_samples),
			(float)i / (float)num_circle_samples,
			(float)i / (float)num_circle_samples);
		vertex_coord = glm::vec2(1.0 - ((float)i / (float)num_circle_samples), (float)i / (float)num_circle_samples);

		// Add vectors to the data buffer
		for (int k = 0; k < 3; k++) {
			vertex[num_circle_samples * vertex_att + i * vertex_att + k] = vertex_position[k];
			vertex[num_circle_samples * vertex_att + i * vertex_att + k + 3] = vertex_normal[k];
			vertex[num_circle_samples * vertex_att + i * vertex_att + k + 6] = vertex_color[k];
		}
		vertex[i*vertex_att + 9] = vertex_coord[0];
		vertex[i*vertex_att + 10] = vertex_coord[1];
	}

	// Create triangles for second circle
	for (int i = 1; i < num_circle_samples; i++) {

		//triangle from first vertex to current vertex and next one
		glm::vec3 t(num_circle_samples, ((i + 1) % num_circle_samples) + num_circle_samples, i + num_circle_samples);

		// Add triangle to the data buffer
		for (int k = 0; k < 3; k++)
		{
			face[(i + num_circle_samples)*face_att + k] = (GLuint)t[k];
		}
	}


	// Create triangles the connect the first and second circles, aka the sides of the cylinder
	for (int i = 0; i < num_circle_samples; i++)
	{
		// Visit every vertex and create 2 triangles
		// first triangle connects 2 vertices from first circle to 1 from second circle
		// second triangle connects 1 vertex from first circle to 2 from second circle

		glm::vec3 t1(
			i,
			i + num_circle_samples,
			(i + 1) % num_circle_samples
		);

		glm::vec3 t2(
			((i + 1) % num_circle_samples) + num_circle_samples,
			(i + 1) % num_circle_samples,
			i + num_circle_samples
		);

		// Add two triangles to the data buffer
		for (int k = 0; k < 3; k++) {
			face[(i + 2 * num_circle_samples)*face_att + k] = (GLuint)t1[k];
			face[(i + 3 * num_circle_samples)*face_att + k] = (GLuint)t2[k];
		}
	}

	// Create OpenGL buffers and copy data
   //GLuint vao;
   //glGenVertexArrays(1, &vao);
   //glBindVertexArray(vao);

	GLuint vbo, ebo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex_num * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_num * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);

	// Free data buffers
	delete[] vertex;
	delete[] face;

	// Create resource
	AddResource(Mesh, object_name, vbo, ebo, face_num * face_att);
}

void ResourceManager::CreateSquare(std::string object_name, float width /*= 1.0*/)
{
	// Create a sphere using a well-known parameterization

	// Number of vertices and faces to be created
	const GLuint vertex_num = 4;
	const GLuint face_num = 2;

	// Number of attributes for vertices and faces
	const int vertex_att = 11;
	const int face_att = 3;

	float halfWidth = width / 2.0f;

	// The construction does *not* use shared vertices
	// 9 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3), Tex Co-ord(2)
	GLfloat vertex[] = {
		-halfWidth, -halfWidth,  0.0,	  0.0,  0.0,  1.0,    1.0, 0.0, 0.0,		0.0, 0.0,
		 halfWidth, -halfWidth,  0.0,	  0.0,  0.0,  1.0,    0.0, 1.0, 0.0,		0.0, 0.0,
		 -halfWidth,  halfWidth, 0.0,	  0.0,  0.0,  1.0,    0.0, 1.0, 1.0,		0.0, 0.0,
		 halfWidth,  halfWidth,  0.0,	  0.0,  0.0,  1.0,    0.0, 0.0, 1.0,		0.0, 0.0,
	};

	// Faces that form the square
	// Every three indices define one triangle
	GLuint face[] = {
		0, 1, 2,
		1, 2, 3,
	};

	// Create OpenGL buffers and copy data
   //GLuint vao;
   //glGenVertexArrays(1, &vao);
   //glBindVertexArray(vao);

	GLuint vbo, ebo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex_num * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_num * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);

	// Create resource
	AddResource(Mesh, object_name, vbo, ebo, face_num * face_att);
}

} // namespace game;
