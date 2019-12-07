

#include "projectile_node.h"
#include "player_node.h"


#include <typeinfo>
#include <GLFW/glfw3.h>



namespace game
{
ProjectileNode::ProjectileNode(std::string name, const Resource *geometry, const Resource *material, float lifespan, glm::vec3 initialPos, glm::vec3 initialVelocityVec, const Resource *texture /*= NULL*/)
	: EntityNode(name, geometry, material, texture)
	, mRemainingLife(lifespan)
	, mLastTime(glfwGetTime())
{
}



ProjectileNode::~ProjectileNode()
{

}

void ProjectileNode::Update()
{
	EntityNode::Update();

	for (BaseNode* bn : getChildNodes())
	{
		bn->Update();
	}

	// Check to see if the projectile is still alive, if not destroy it
	double currentTime = glfwGetTime();
	if ((currentTime - mLastTime) > 0.05) 
	{
		mRemainingLife -= (currentTime - mLastTime);
		mLastTime = currentTime;
	}

	if (mRemainingLife <= 0.0f)
	{
		mParentNode->removeChildNode(getName());
		return;
	}
}

HeatMissileNode::HeatMissileNode(std::string name, const Resource *geometry, const Resource *material, float lifespan, glm::vec3 initialPos, glm::vec3 initialVelocityVec, const Resource *texture /*= NULL*/)
	:ProjectileNode(name, geometry, material, lifespan, initialPos, initialVelocityVec, texture)
{
	mPosition = initialPos;
	mVelocity = initialVelocityVec;
	mMaxVelocity = glm::length(initialVelocityVec);
}

HeatMissileNode::~HeatMissileNode()
{

}

void HeatMissileNode::Update()
{
	ProjectileNode::Update();


	// Get the player pos
	glm::vec3 playerPos = getPlayerPosition();

	glm::vec3 dirPlayer = playerPos - mPosition;
	Rotate(dirPlayer);

	mVelocity += 0.2f * dirPlayer;
	mVelocity = mMaxVelocity * glm::normalize(mVelocity);


	if (glm::distance(mPosition, playerPos) < 0.5f)
	{
		PlayerNode* playerNode = getPlayerNode();
		playerNode->takeDamage(MISSILE);

		mDeleteNextTick = true;
	}
}

}