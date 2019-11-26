#include "entity_game_nodes.h"

namespace game
{


CowEntityNode::CowEntityNode(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture /*= NULL*/) 
	: EntityNode(name, geometry, material, texture)
	, mLastTimer(0.0f)
	, mNextTimer(0.0f)
	, mBehaviour(stand)
{

}

CowEntityNode::~CowEntityNode()
{

}

void CowEntityNode::Update()
{
	EntityNode::Update();

	// Basic Cow should walk around occasionally, and stop occasionally
	// This behaviors repeats indefinitely
	// If a cow is picked up and then dropped, it should run around erratically for some time, before stopping and continuing normal behavior

	//Timer Stuff
	float currentTime = glfwGetTime();
	if (currentTime >= mNextTimer)
	{
		if (mNextTimer != 0.0f)
		{
			if (mBehaviour == stand)
				mBehaviour = walk;
			else if (mBehaviour == walk)
				mBehaviour = stand;
			else if (mBehaviour == run)
				mBehaviour = stand;
		}

		mLastTimer = currentTime;

		float minPeriod = 2.0f;
		float maxPeriod = 6.0f;

		mNextTimer = currentTime + minPeriod + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / maxPeriod - minPeriod);
	}

	// Behaviour Stuff
	if (mBehaviour == stand)
	{
		mVelocity = glm::vec3(0.0f);
	}
	else if (mBehaviour == walk)
	{
		if (mVelocity == glm::vec3(0.0f))
		{
			glm::vec3 dirVec = glm::vec3(
				-1.0f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (1.0f - (-1.0f))),
				0.0f,
				-1.0f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (1.0f - (-1.0f)))
			);

			mVelocity = 0.2f * glm::normalize(dirVec);

			Rotate(mVelocity);
		}
	}
	else if (mBehaviour == run)
	{
		glm::vec3 dirVec = glm::vec3(
			-1.0f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (1.0f - (-1.0f))),
			0.0f,
			-1.0f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (1.0f - (-1.0f)))
		);

		mVelocity += 0.1f * glm::normalize(dirVec);
		mVelocity = 0.5f * glm::normalize(mVelocity);
		Rotate(mVelocity);
	}
	// todo Dropped Erratic behavior

	
}

}


