#include <stdexcept>
#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <time.h>


#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

#include "scene_node.h"

namespace game {
	SceneNode::SceneNode(const std::string name) : BaseNode(name)
	{
	}

	SceneNode::SceneNode(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture)
	: BaseNode(name)
{
    // Set geometry
    if (geometry->getType() == PointSet){
        mMode = GL_POINTS;
    } else if (geometry->getType() == Mesh){
        mMode = GL_TRIANGLES;
    } else {
        throw(std::invalid_argument(std::string("Invalid type of geometry")));
    }

    mArrayBuffer = geometry->getArrayBuffer();
    mElementArrayBuffer = geometry->getElementArrayBuffer();
    mSize = geometry->getSize();

    // Set material (shader program)
    if (material->getType() != Material){
        throw(std::invalid_argument(std::string("Invalid type of material")));
    }

	mMaterial = material->getResource();

	// Set texture
	if (texture) {
		mTexture = texture->getResource();
	}
	else {
		mTexture = 0;
	}

    // Other attributes
    mScale = glm::vec3(1.0, 1.0, 1.0);
	radius = 1.0;
	collisionType = Point;
	gridPosition = glm::vec2(floor(mPosition.x / 15), floor(mPosition.x / 15));
}


SceneNode::~SceneNode(){
}



glm::vec3 SceneNode::getPosition(void) {

    return mPosition;
}


glm::quat SceneNode::getOrientation(void) const {

    return mOrientation;
}


glm::vec3 SceneNode::getscale(void) const {

    return mScale;
}


void SceneNode::setPosition(glm::vec3 position){

    mPosition = position;
}


void SceneNode::setOrientation(glm::quat orientation){

    mOrientation = orientation;
}


void SceneNode::setScale(glm::vec3 scale){

    mScale = scale;
}

void SceneNode::setGridPosition(glm::vec3 pos)
{
	gridPosition.x = floor(pos.x / 15);
	gridPosition.y = floor(pos.z / 15);
}


void SceneNode::translate(glm::vec3 trans){

    mPosition += trans;
}


void SceneNode::rotate(glm::quat rot){

    mOrientation *= rot;
    mOrientation = glm::normalize(mOrientation);
}

// Code adapted from https://github.com/opengl-tutorials/ogl/blob/master/common/quaternion_utils.cpp
void SceneNode::rotate(glm::vec3 direction)
{
	
	glm::vec3 upVec = glm::vec3(0.0f, 1.0f, 0.0f);

	if (glm::length2(direction) < 0.0001f)
		//return glm::quat();
		return;

	// Recompute desiredUp so that it's perpendicular to the direction
	// You can skip that part if you really want to force desiredUp
	glm::vec3 right = glm::cross(direction, upVec);
	upVec = glm::cross(right, direction);

	// Find the rotation between the front of the object (that we assume towards +Z,
	// but this depends on your model) and the desired direction
	glm::quat rot1 = QuatBetweenVectors(glm::vec3(0.0f, 0.0f, 1.0f), direction);
	// Because of the 1rst rotation, the up is probably completely screwed up. 
	// Find the rotation between the "up" of the rotated object, and the desired up
	glm::vec3 newUp = rot1 * glm::vec3(0.0f, 1.0f, 0.0f);
	glm::quat rot2 = QuatBetweenVectors(newUp, upVec);

	// Apply them
	//return rot2 * rot1; // remember, in reverse order.
	setOrientation(rot1 * rot2);

}

void SceneNode::scale(glm::vec3 scale){

    mScale *= scale;
}



GLenum SceneNode::getMode(void) const {

    return mMode;
}


GLuint SceneNode::getArrayBuffer(void) const {

    return mArrayBuffer;
}


GLuint SceneNode::getElementArrayBuffer(void) const {

    return mElementArrayBuffer;
}


GLsizei SceneNode::getSize(void) const {

    return mSize;
}


GLuint SceneNode::getMaterial(void) const {

    return mMaterial;
}

GLuint SceneNode::getTexture(void) const {

	return mTexture;
}


void SceneNode::draw(SceneNode *camera, glm::mat4 parentTransf){

	// Select proper material (shader program)
	glUseProgram(mMaterial);

	// Set geometry to draw
	glBindBuffer(GL_ARRAY_BUFFER, mArrayBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementArrayBuffer);

	// Set globals for camera
	camera->SetupShader(mMaterial);

	// Set world matrix and other shader input variables
	SetupShader(mMaterial, parentTransf);

	// draw geometry
	if (mMode == GL_POINTS) {
		glDrawArrays(mMode, 0, mSize);
	}
	else {
		glDrawElements(mMode, mSize, GL_UNSIGNED_INT, 0);
	}

	for (BaseNode* bn : getChildNodes())
	{
		dynamic_cast<SceneNode*>(bn)->draw(camera, parentTransf);
	}
	
}


void SceneNode::update(double deltaTime)
{
	mPosition = glm::clamp(mPosition, 0.0f, 300.0f); // clamp to map limits
	gridPosition = glm::vec2(floor(mPosition.x / 15), floor(mPosition.z / 15));

	for (BaseNode* bn : getChildNodes())
	{
		bn->update(deltaTime);
	}

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
	glm::mat4 rotation = glm::mat4_cast(mOrientation);
	glm::mat4 translation = glm::translate(parentTransf, mPosition);

	parentTransf = translation * rotation;

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

// Source code from https://github.com/opengl-tutorials/ogl/blob/master/common/quaternion_utils.cpp
glm::quat SceneNode::QuatBetweenVectors(glm::vec3 start, glm::vec3 dest)
{
	start = glm::normalize(start);
	dest = glm::normalize(dest);

	float cosTheta = glm::dot(start, dest);
	glm::vec3 rotationAxis;

	if (cosTheta < -1 + 0.001f) {
		// special case when vectors in opposite directions :
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		// This implementation favors a rotation around the Up axis,
		// since it's often what you want to do.
		rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
		if (glm::length2(rotationAxis) < 0.01) // bad luck, they were parallel, try again!
			rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

		rotationAxis = glm::normalize(rotationAxis);
		return glm::angleAxis(glm::radians(180.0f), rotationAxis);
	}

	// Implementation from Stan Melax's Game Programming Gems 1 article
	rotationAxis = glm::cross(start, dest);

	float s = sqrt((1 + cosTheta) * 2);
	float invs = 1 / s;

	return glm::quat(
		s * 0.5f,
		rotationAxis.x * invs,
		rotationAxis.y * invs,
		rotationAxis.z * invs
	);
}

} // namespace game;
