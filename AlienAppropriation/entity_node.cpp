#include "entity_node.h"

namespace game
{


EntityNode::EntityNode(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture /*= NULL*/) 
	: SceneNode(name, geometry, material, texture)
{

}

EntityNode::~EntityNode()
{

}



}


