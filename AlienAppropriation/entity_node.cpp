
#include "entity_node.h"
#include "player_node.h"

namespace game
{


EntityNode::EntityNode(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture)
	: SceneNode(name, geometry, material, texture)
	, mVelocity(glm::vec3(0.0f,0.0f,0.0f))
	, mAcceleration(glm::vec3(0.0f, 0.0f, 0.0f))
	, mIsGrounded(true)
{

}

EntityNode::~EntityNode()
{

}



void EntityNode::update(double deltaTime)
{
	//Apply Gravity
	if (!mIsGrounded)
	{
		if (mPosition.y > 0.0f)
		{
			mVelocity += GRAVITY;
			mVelocity.x = 0.0f;
			mVelocity.z = 0.0f;
		}
		else
		{
			mVelocity.y = 0.0f;
			mPosition.y = 0.0f;
			mIsGrounded = true;
			hitGround();
		}
	}
	else
		mVelocity.y = std::max(mVelocity.y, 0.0f);

	mPosition += mVelocity;

	SceneNode::update(deltaTime);

}

void EntityNode::rise(glm::vec3 dir)
{
 	if (mPosition.y == 0.0f)
		mPosition.y = 0.1f;
	mVelocity += glm::vec3(0.0, 0.2, 0.0) + -GRAVITY;
	setIsGrounded(false);
}

void EntityNode::hitGround()
{

}

}


