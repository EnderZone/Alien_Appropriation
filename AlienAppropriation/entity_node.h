#ifndef ENTITY_NODE_H_
#define ENTITY_NODE_H_


#include "scene_node.h"

namespace game {

	// Class that manages one object in a scene 
	class EntityNode : public SceneNode {

	public:
		// Create scene node from given resources
		EntityNode(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture = NULL);

		// Destructor
		~EntityNode();

	}; // class EntityNode

} // namespace game

#endif // ENTITY_NODE_H_
