#ifndef ENTITY_NODE_H_
#define ENTITY_NODE_H_

#include <math.h>
#include <algorithm>

#include "scene_node.h"

#define GRAVITY glm::vec3(0.0f, -0.01f, 0.0f)

namespace game {

	

	// Class that manages one object in a scene 
	class EntityNode : public SceneNode {

	public:
		// Create scene node from given resources
		EntityNode(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture = NULL);

		// Destructor
		~EntityNode();

		virtual void Update();

	protected:

		glm::vec3 getPlayerPosition();

		// mPosition is stored in SceneNode
		glm::vec3 mVelocity;
		glm::vec3 mAcceleration;

		bool mIsGrounded;





	}; // class EntityNode

} // namespace game

#endif // ENTITY_NODE_H_
