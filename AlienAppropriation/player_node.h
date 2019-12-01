#pragma once

#include "scene_node.h"
#include "base_node.h"
#include "camera.h"

#include <string.h>

namespace game
{	
	class PlayerNode : SceneNode
	{
	public:
		PlayerNode(const std::string name, const Resource *geometry, const Resource *material, BaseNode* camera);
		~PlayerNode();

		void rotateLeft();
		void rotateRight();
		void rotateForward();
		void rotateBackward();

		void rotateByCamera();

		void Draw(Camera* camera, glm::mat4 parentTransf = glm::mat4(1.0));
		void Update(void);

	private:
		float x_tilt_percentage;
		float y_tilt_percentage;

		void SetupShader(GLuint program, glm::mat4& parentTransf = glm::mat4(1.0));
	};
}
