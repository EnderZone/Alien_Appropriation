
#include "entity_node.h"

namespace game
{


EntityNode::EntityNode(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture /*= NULL*/) 
	: SceneNode(name, geometry, material, texture)
	, mVelocity(glm::vec3(0.0f,0.0f,0.0f))
	, mAcceleration(glm::vec3(0.0f, 0.0f, 0.0f))
	, mIsGrounded(true)
{

}

EntityNode::~EntityNode()
{

}



void EntityNode::Update()
{
	SceneNode::Update();

	//Apply Gravity

	if (!mIsGrounded)
		mVelocity += GRAVITY;
	else
		mVelocity.y = std::max(mVelocity.y, 0.0f);

	mPosition += mVelocity;

	//todo Friction

}

}


