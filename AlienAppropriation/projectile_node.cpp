

#include "projectile_node.h"
#include "player_node.h"
#include "scene_graph.h"


#include <typeinfo>
#include <GLFW/glfw3.h>



namespace game
{
ProjectileNode::ProjectileNode(std::string name, const Resource *geometry, const Resource *material, float lifespan, glm::vec3 initialPos, glm::vec3 initialVelocityVec, const Resource *texture /*= NULL*/)
	: EntityNode(name, geometry, material, texture)
	, mRemainingLife(lifespan)
	, mLastTime(glfwGetTime())
{
	addTag("projectile");
}



ProjectileNode::~ProjectileNode()
{

}

void ProjectileNode::update(double deltaTime)
{
	EntityNode::update(deltaTime);

	// Check to see if the projectile is still alive, if not destroy it
	double currentTime = glfwGetTime();
	if ((currentTime - mLastTime) > 0.05) 
	{
		mRemainingLife -= (currentTime - mLastTime);
		mLastTime = currentTime;
	}

	if (mRemainingLife <= 0.0f)
	{
		addTag("delete");
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

void HeatMissileNode::update(double deltaTime)
{
	ProjectileNode::update(deltaTime);

	// Get the player pos
	glm::vec3 playerPos = SceneGraph::getPlayerNode()->getPosition();

	glm::vec3 dirPlayer = playerPos - mPosition;
	rotate(dirPlayer);

	mVelocity += 0.2f * dirPlayer;
	mVelocity = mMaxVelocity * glm::normalize(mVelocity);
}

}