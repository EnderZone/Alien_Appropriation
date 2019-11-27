#include "player_node.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace game {
	PlayerNode::PlayerNode(const std::string name, const Resource *geometry, const Resource *material, BaseNode* camera) : SceneNode(name, geometry, material) {
		// Set This as the parentNode of the camera while taking its own parent as his
		this->setParentNode(camera->getParentNode());
		camera->setParentNode(this);
		this->addChildNode(camera);
	}

	PlayerNode::~PlayerNode() {}

	void PlayerNode::addRotationX() {
	}

	void PlayerNode::addRotationY() {
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

		// Aply transformations *ISROT*
		glm::mat4 scaling = glm::scale(glm::mat4(1.0), mScale);
		glm::mat4 rotation = glm::mat4_cast(mOrientation);
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