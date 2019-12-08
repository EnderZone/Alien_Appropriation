
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

void EntityNode::rise()
{
 	if (mPosition.y == 0.0f)
		mPosition.y = 0.1f;
	mVelocity += glm::vec3(0.0, 0.5, 0.0) + -GRAVITY;
	setIsGrounded(false);
}

//game::PlayerNode* EntityNode::getPlayerNode()
//{
//	// Get the root node
//	BaseNode* rootNode = this;
//	while (rootNode->getName() != "ROOT")
//	{
//		rootNode = rootNode->getParentNode();
//	}
//
//	if (!rootNode)
//		throw("Root Node could not be found from " + getName());
//
//	Camera* cameraNode;
//	for (BaseNode* m : rootNode->getChildNodes())
//	{
//		if (m->getName() == "CAMERA")
//		{
//			cameraNode = reinterpret_cast<Camera*>(m);
//			break;
//		}
//	}
//
//	if (!cameraNode)
//		throw("Camera Node could not be found from " + getName());
//
//	PlayerNode* playerNode;
//	for (BaseNode* m : cameraNode->getChildNodes())
//	{
//		if (m->getName() == "PLAYER")
//		{
//			playerNode = reinterpret_cast<PlayerNode*>(m);
//			break;
//		}
//	}
//
//	if (!playerNode)
//		throw("Player Node could not be found from " + getName());
//
//	return playerNode;
//}
//
//glm::vec3 EntityNode::getPlayerPosition()
//{
//	return getPlayerNode()->getPosition();
//}

void EntityNode::hitGround()
{

}

}


