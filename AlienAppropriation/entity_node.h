#ifndef ENTITY_NODE_H_
#define ENTITY_NODE_H_

#include <math.h>
#include <algorithm>

#include "scene_node.h"

#define GRAVITY glm::vec3(0.0f, -1.0f, 0.0f)

namespace game {

	// class EntityNode
	// A node with some movement behaviour
	class EntityNode : public SceneNode {

	public:
		// Create scene node from given resources
		EntityNode(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture = NULL);

		// Destructor
		~EntityNode();

		virtual void update(double deltaTime);

		void rise(glm::vec3 dir);

		inline bool getIsGrounded() { return mIsGrounded; }
		inline void setIsGrounded(bool b) { mIsGrounded = b; }

	protected:

		//PlayerNode* getPlayerNode();
		//glm::vec3 getPlayerPosition();

		// mPosition is stored in SceneNode
		glm::vec3 mVelocity;
		glm::vec3 mAcceleration;

		bool mIsGrounded;

	private:
		virtual void hitGround();



	}; // class EntityNode

} // namespace game

#endif // ENTITY_NODE_H_
