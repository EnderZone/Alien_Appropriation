#include <stdexcept>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "camera.h"
#include "scene_node.h"
#include "scene_graph.h"

namespace game {

Camera::Camera(std::string name)
	: SceneNode(name)
	, mCameraPerspective(First)
{
}

Camera::~Camera(){
}

glm::vec3 Camera::GetForward(void) const {

    glm::vec3 current_forward = mOrientation * mForward;
    return -current_forward; // Return -forward since the camera coordinate system points in the opposite direction
}

glm::vec3 Camera::GetSide(void) const {

    glm::vec3 current_side = mOrientation * mSide;
    return current_side;
}


glm::vec3 Camera::GetUp(void) const {
    glm::vec3 current_forward = mOrientation * mForward;
    glm::vec3 current_side = mOrientation * mSide;
    glm::vec3 current_up = glm::cross(current_forward, current_side);
    current_up = glm::normalize(current_up);
    return current_up;
}


void Camera::draw(SceneNode *camera, glm::mat4 parentTransf)
{
	// The camera cannot be drawn - instead the function passes the camera's transform to its children
	glm::mat4 rotation = glm::mat4_cast(mOrientation);
	glm::mat4 translation = glm::translate(parentTransf, mPosition);

	parentTransf = translation;
	
	for (BaseNode* bn : getChildNodes())
	{
		if (mCameraPerspective == First) {
			dynamic_cast<SceneNode*>(bn)->draw(camera, parentTransf);
		}
	}
}

void Camera::update(double deltaTime)
{
	mPosition += mVelocity.x * glm::cross(playerForward, glm::vec3(0.0f, -1.0f, 0.0f));
	mVelocity.x *= 0.95;

	mPosition += mVelocity.y * glm::vec3(0.0f, 1.0f, 0.0f);
	mVelocity.y *= 0.95;

	mPosition += -mVelocity.z * playerForward;
	mVelocity.z *= 0.95;

	mPosition.x = glm::clamp(mPosition.x, 0.0f, 300.0f);
	mPosition.y = glm::clamp(mPosition.y, 5.0f, 50.0f);
	mPosition.z = glm::clamp(mPosition.z, 40.0f, 340.0f);

	for (BaseNode* bn : getChildNodes())
	{
		bn->update(deltaTime);
	}
}

void Camera::SwitchCameraPerspective()
{
	glm::vec3 camShiftVec = glm::vec3(0.0f, 0.0f, (SceneGraph::getPlayerNode()->getPosition() - mPosition).z);

	if (mCameraPerspective == First)
	{
		mCameraPerspective = Third;
		translate(((GetUp() * camShiftVec.y) + (GetForward() * -camShiftVec.z)));
		for (BaseNode* bn : getChildNodes())
		{
			if (SceneNode* sn = dynamic_cast<SceneNode*>(bn))
				sn->translate(-camShiftVec);
		}
	}
	else if (mCameraPerspective == Third)
	{
		mCameraPerspective = First;
		translate(((GetUp() * -camShiftVec.y) + (GetForward() * camShiftVec.z)));
		for (BaseNode* bn : getChildNodes())
		{
			if (SceneNode* sn = dynamic_cast<SceneNode*>(bn))
				sn->translate(camShiftVec);
		}
	}
}

void Camera::Pitch(float angle){
	glm::quat rotation = glm::angleAxis(angle, GetSide());
    mOrientation = rotation * mOrientation;
    mOrientation = glm::normalize(mOrientation);
}


void Camera::Yaw(float angle){
	glm::quat rotation = glm::angleAxis(angle, glm::vec3(0.0, 1.0, 0.0));
	mOrientation = rotation * mOrientation;
	mOrientation = glm::normalize(mOrientation);
	
	glm::quat movement_rot = glm::angleAxis(angle, glm::vec3(0.0f, -1.0f, 0.0f));
	playerForward = playerForward * glm::mat3_cast(movement_rot);
	playerForward = glm::normalize(playerForward);
}


void Camera::Roll(float angle){
	glm::quat rotation = glm::angleAxis(angle, GetForward());
	mOrientation = rotation * mOrientation;
	mOrientation = glm::normalize(mOrientation);
}


void Camera::SetView(glm::vec3 position, glm::vec3 look_at, glm::vec3 up){

    // Store initial forward and side vectors
    // See slide in "Camera control" for details
    mForward = look_at - position;
    mForward = -glm::normalize(mForward);
    mSide = glm::cross(up, mForward);
    mSide = glm::normalize(mSide);

    // Reset orientation and position of camera
    mPosition = position;
    mOrientation = glm::quat();
	playerForward = mForward;
	playerForward = glm::normalize(playerForward);
}


void Camera::SetProjection(GLfloat fov, GLfloat near, GLfloat far, GLfloat w, GLfloat h){

    // Set projection based on field-of-view
    float top = tan((fov/2.0)*(glm::pi<float>()/180.0))*near;
    float right = top * w/h;
    mProjectionMatrix = glm::frustum(-right, right, -top, top, near, far);
}


void Camera::SetupShader(GLuint program, glm::mat4& parentTransf){

    // update view matrix
    SetupViewMatrix();

    // Set view matrix in shader
    GLint view_mat = glGetUniformLocation(program, "view_mat");
    glUniformMatrix4fv(view_mat, 1, GL_FALSE, glm::value_ptr(mViewMatrix));
    
    // Set projection matrix in shader
    GLint projection_mat = glGetUniformLocation(program, "projection_mat");
    glUniformMatrix4fv(projection_mat, 1, GL_FALSE, glm::value_ptr(mProjectionMatrix));
}


void Camera::SetupViewMatrix(void){

    // view_matrix_ = glm::lookAt(position, look_at, up);

    // Get current vectors of coordinate system
    // [side, up, forward]
    // See slide in "Camera control" for details
    glm::vec3 current_forward = mOrientation * mForward;
    glm::vec3 current_side = mOrientation * mSide;
    glm::vec3 current_up = glm::cross(current_forward, current_side);
    current_up = glm::normalize(current_up);

    // Initialize the view matrix as an identity matrix
    mViewMatrix = glm::mat4(1.0); 

	// Adding player to the view Matrix
	if (mCameraPerspective == First)
	mViewMatrix = glm::translate(mViewMatrix, SceneGraph::getPlayerNode()->getPosition() - mPosition);

    // Copy vectors to matrix
    // Add vectors to rows, not columns of the matrix, so that we get
    // the inverse transformation
    // Note that in glm, the reference for matrix entries is of the form
    // matrix[column][row]
    mViewMatrix[0][0] = current_side[0]; // First row
    mViewMatrix[1][0] = current_side[1];
    mViewMatrix[2][0] = current_side[2];
    mViewMatrix[0][1] = current_up[0]; // Second row
    mViewMatrix[1][1] = current_up[1];
    mViewMatrix[2][1] = current_up[2];
    mViewMatrix[0][2] = current_forward[0]; // Third row
    mViewMatrix[1][2] = current_forward[1];
    mViewMatrix[2][2] = current_forward[2];

	if (mCameraPerspective == First)
		mViewMatrix = glm::translate(mViewMatrix, -(SceneGraph::getPlayerNode()->getPosition() - mPosition));

    // Create translation to camera position
    glm::mat4 trans = glm::translate(glm::mat4(1.0), -mPosition);

    // Combine translation and view matrix in proper order
    mViewMatrix *= trans;
}

} // namespace game
