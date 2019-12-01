#include <stdexcept>
#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <time.h>

#include "scene_node.h"

namespace game {

SceneNode::SceneNode(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture)
	: BaseNode(name)
{
    // Set geometry
    if (geometry->GetType() == PointSet){
        mMode = GL_POINTS;
    } else if (geometry->GetType() == Mesh){
        mMode = GL_TRIANGLES;
    } else {
        throw(std::invalid_argument(std::string("Invalid type of geometry")));
    }

    mArrayBuffer = geometry->GetArrayBuffer();
    mElementArrayBuffer = geometry->GetElementArrayBuffer();
    mSize = geometry->GetSize();

    // Set material (shader program)
    if (material->GetType() != Material){
        throw(std::invalid_argument(std::string("Invalid type of material")));
    }

	// Set texture
	if (texture) {
		mTexture = texture->GetResource();
	}
	else {
		mTexture = 0;
	}

    mMaterial = material->GetResource();

    // Other attributes
    mScale = glm::vec3(1.0, 1.0, 1.0);
}


SceneNode::~SceneNode(){
}



glm::vec3 SceneNode::GetPosition(void) const {

    return mPosition;
}


glm::quat SceneNode::GetOrientation(void) const {

    return mOrientation;
}


glm::vec3 SceneNode::GetScale(void) const {

    return mScale;
}


void SceneNode::SetPosition(glm::vec3 position){

    mPosition = position;
}


void SceneNode::SetOrientation(glm::quat orientation){

    mOrientation = orientation;
}


void SceneNode::SetScale(glm::vec3 scale){

    mScale = scale;
}


void SceneNode::Translate(glm::vec3 trans){

    mPosition += trans;
}


void SceneNode::Rotate(glm::quat rot){

    mOrientation *= rot;
    mOrientation = glm::normalize(mOrientation);
}


void SceneNode::Scale(glm::vec3 scale){

    mScale *= scale;
}



GLenum SceneNode::GetMode(void) const {

    return mMode;
}


GLuint SceneNode::GetArrayBuffer(void) const {

    return mArrayBuffer;
}


GLuint SceneNode::GetElementArrayBuffer(void) const {

    return mElementArrayBuffer;
}


GLsizei SceneNode::GetSize(void) const {

    return mSize;
}


GLuint SceneNode::GetMaterial(void) const {

    return mMaterial;
}


void SceneNode::Draw(Camera *camera, glm::mat4 parentTransf /*= glm::mat4(1.0)*/){

    // Select proper material (shader program)
    glUseProgram(mMaterial);

    // Set geometry to draw
    glBindBuffer(GL_ARRAY_BUFFER, mArrayBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementArrayBuffer);

    // Set globals for camera
    camera->SetupShader(mMaterial);

    // Set world matrix and other shader input variables
    SetupShader(mMaterial, parentTransf);

    // Draw geometry
    if (mMode == GL_POINTS){
        glDrawArrays(mMode, 0, mSize);
    } else {
        glDrawElements(mMode, mSize, GL_UNSIGNED_INT, 0);
    }

	for (BaseNode* bn : getChildNodes())
	{
		bn->Draw(camera, parentTransf);
	}
	
}


void SceneNode::Update(void)
{
	for (BaseNode* bn : getChildNodes())
	{
		bn->Update();
	}

    // Do nothing specific for this generic type of scene node
}


void SceneNode::SetupShader(GLuint program, glm::mat4& parentTransf /*= glm::mat4(1.0)*/){

    // Set attributes for shaders
    GLint vertex_att = glGetAttribLocation(program, "vertex");
    glVertexAttribPointer(vertex_att, 3, GL_FLOAT, GL_FALSE, 11*sizeof(GLfloat), 0);
    glEnableVertexAttribArray(vertex_att);

    GLint normal_att = glGetAttribLocation(program, "normal");
    glVertexAttribPointer(normal_att, 3, GL_FLOAT, GL_FALSE, 11*sizeof(GLfloat), (void *) (3*sizeof(GLfloat)));
    glEnableVertexAttribArray(normal_att);

    GLint color_att = glGetAttribLocation(program, "color");
    glVertexAttribPointer(color_att, 3, GL_FLOAT, GL_FALSE, 11*sizeof(GLfloat), (void *) (6*sizeof(GLfloat)));
    glEnableVertexAttribArray(color_att);

    GLint tex_att = glGetAttribLocation(program, "uv");
    glVertexAttribPointer(tex_att, 2, GL_FLOAT, GL_FALSE, 11*sizeof(GLfloat), (void *) (9*sizeof(GLfloat)));
    glEnableVertexAttribArray(tex_att);

	// Aply transformations *ISROT*
	glm::mat4 scaling = glm::scale(glm::mat4(1.0), mScale);
	glm::mat4 rotation = glm::mat4_cast(mOrientation);
	glm::mat4 translation = glm::translate(parentTransf, mPosition);

	parentTransf = translation * rotation;

	// Scaling is done only on local object
	//glm::mat4 transf = glm::scale(parentTransf, mScale);

    GLint world_mat = glGetUniformLocation(program, "world_mat");
    glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(glm::scale(parentTransf, mScale)));
	 // Normal matrix
    glm::mat4 normal_matrix = glm::transpose(glm::inverse(parentTransf));
    GLint normal_mat = glGetUniformLocation(program, "normal_mat");
    glUniformMatrix4fv(normal_mat, 1, GL_FALSE, glm::value_ptr(normal_matrix));


	// Texture
	if (mTexture) {
		GLint tex = glGetUniformLocation(program, "texture_map");
		glUniform1i(tex, 0); // Assign the first texture to the map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mTexture); // First texture we bind
		// Define texture interpolation
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

    // Timer
    GLint timer_var = glGetUniformLocation(program, "timer");
    double current_time = glfwGetTime();
    glUniform1f(timer_var, (float) current_time);
}

} // namespace game;
