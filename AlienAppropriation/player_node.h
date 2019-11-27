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
		
		void addRotationX();
		void addRotationY();

		void Draw(Camera* camera, glm::mat4 parentTransf = glm::mat4(1.0));
	
	private:
		float x_angle;
		float y_angle;

		void SetupShader(GLuint program, glm::mat4& parentTransf = glm::mat4(1.0));
	};
}
