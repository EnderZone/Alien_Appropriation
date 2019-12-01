#include "player_node.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace game {
	PlayerNode::PlayerNode(const std::string name, const Resource *geometry, const Resource *material, BaseNode* camera) : SceneNode(name, geometry, material) {
		// Set This as the parentNode of the camera while taking its own parent as his
		this->setParentNode(camera->getParentNode());
		camera->setParentNode(this);
		this->addChildNode(camera);

		x_tilt_percentage = 0;
		y_tilt_percentage = 0;
	}

	PlayerNode::~PlayerNode() {}

	void PlayerNode::rotateLeft() {
		x_tilt_percentage += glm::pi<float>() / 20.0f;
		x_tilt_percentage = glm::min(glm::pi<float>() / 2.0f, x_tilt_percentage);
	}

	void PlayerNode::rotateRight() {
		x_tilt_percentage -= glm::pi<float>() / 20.0f;
		x_tilt_percentage = glm::max(-glm::pi<float>() / 2.0f, x_tilt_percentage);
	}

	void PlayerNode::rotateForward() {
		y_tilt_percentage += glm::pi<float>() / 20.0f;
		y_tilt_percentage = glm::min(glm::pi<float>() / 2.0f, y_tilt_percentage);
	}

	void PlayerNode::rotateBackward() {
		y_tilt_percentage -= glm::pi<float>() / 20.0f;
		y_tilt_percentage = glm::max(-glm::pi<float>() / 2.0f, y_tilt_percentage);
	}

	void PlayerNode::rotateByCamera() {
		float velocity_limit = glm::pi<float>() / 2.0f;

		x_tilt_percentage = -((Camera*)this->getChildNodes()[0])->getVelocitySide() * velocity_limit;
		y_tilt_percentage = -((Camera*)this->getChildNodes()[0])->getVelocityForward() * velocity_limit;
	
		x_tilt_percentage = glm::max(-1.0f, x_tilt_percentage);
		x_tilt_percentage = glm::min( 1.0f, x_tilt_percentage);
		
		y_tilt_percentage = glm::max(-1.0f, y_tilt_percentage);
		y_tilt_percentage = glm::min( 1.0f, y_tilt_percentage);
	}

	void PlayerNode::Draw(Camera* camera, glm::mat4 parentTransf) {
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
		if (mMode == GL_POINTS) {
			glDrawArrays(mMode, 0, mSize);
		}
		else {
			glDrawElements(mMode, mSize, GL_UNSIGNED_INT, 0);
		}

		for (BaseNode* bn : getChildNodes())
		{
			bn->Draw(camera, parentTransf);
		}
	}

	void PlayerNode::Update() {
		SceneNode::Update();
		rotateByCamera();
	}

	void PlayerNode::SetupShader(GLuint program, glm::mat4& parentTransf /*= glm::mat4(1.0)*/) {

		// Set attributes for shaders
		GLint vertex_att = glGetAttribLocation(program, "vertex");
		glVertexAttribPointer(vertex_att, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(vertex_att);

		GLint normal_att = glGetAttribLocation(program, "normal");
		glVertexAttribPointer(normal_att, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(normal_att);

		GLint color_att = glGetAttribLocation(program, "color");
		glVertexAttribPointer(color_att, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void *)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(color_att);

		GLint tex_att = glGetAttribLocation(program, "uv");
		glVertexAttribPointer(tex_att, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void *)(9 * sizeof(GLfloat)));
		glEnableVertexAttribArray(tex_att);

		// Adding the tilts when moving
		float angle_x = (glm::pi<float>() / 8) * glm::sin(x_tilt_percentage);
		float angle_y = (glm::pi<float>() / 8) * glm::sin(y_tilt_percentage);
		

		glm::quat current_rotation = mOrientation;
		current_rotation *= glm::quat_cast(glm::rotate(glm::mat4(), angle_x, glm::vec3(0.0, 0.0, 1.0)));
		current_rotation = glm::normalize(current_rotation);
		current_rotation *= glm::quat_cast(glm::rotate(glm::mat4(), angle_y, glm::vec3(1.0, 0.0, 0.0)));
		current_rotation = glm::normalize(current_rotation);

		// Aply transformations *ISROT*
		glm::mat4 scaling = glm::scale(glm::mat4(1.0), mScale);
		glm::mat4 rotation = glm::mat4_cast(current_rotation);
		glm::mat4 translation = glm::translate(parentTransf, mPosition);
		
		translation = glm::translate(translation, ((Camera*)mChildNodes[0])->GetPosition());

		parentTransf = translation * rotation;

		// Scaling is done only on local object
		glm::mat4 transf = glm::scale(parentTransf, mScale);

		GLint world_mat = glGetUniformLocation(program, "world_mat");
		glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));

		// Timer
		GLint timer_var = glGetUniformLocation(program, "timer");
		double current_time = glfwGetTime();
		glUniform1f(timer_var, (float)current_time);
	}

}