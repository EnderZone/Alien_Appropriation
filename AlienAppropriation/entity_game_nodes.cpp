#include "entity_game_nodes.h"

#include "player_node.h"

namespace game
{


CowEntityNode::CowEntityNode(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture /*= NULL*/) 
	: EntityNode(name, geometry, material, texture)
	, mLastTimer(0.0f)
	, mNextTimer(0.0f)
{

	int defaultBehaviour = rand() % 2;
	switch (defaultBehaviour)
	{
	case 0:
		mBehaviour = stand;
		break;
	case 1:
		mBehaviour = walk;
		break;
	}

	// Behaviour override
	//mBehaviour = run;
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
		glm::vec3 dirVec = glm::vec3(
			-1.0f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (1.0f - (-1.0f))),
			0.0f,
			-1.0f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (1.0f - (-1.0f)))
		);

		mVelocity += 0.02f * glm::normalize(dirVec);
		mVelocity = 0.2f * glm::normalize(mVelocity);
		Rotate(mVelocity);
	}
	else if (mBehaviour == run)
	{
		glm::vec3 dirVec = glm::vec3(
			-1.0f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (1.0f - (-1.0f))),
			0.0f,
			-1.0f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (1.0f - (-1.0f)))
		);

		mVelocity += 0.2f * glm::normalize(dirVec);
		mVelocity = 0.5f * glm::normalize(mVelocity);
		Rotate(mVelocity);
	}

	// When dropped, we will manually set mNextTimer

	
}

BullEntityNode::BullEntityNode(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture /*= NULL*/)
	: EntityNode(name, geometry, material, texture)
	, mLastTimer(0.0f)
	, mNextTimer(0.0f)
{
	int defaultBehaviour = rand() % 2;
	switch (defaultBehaviour)
	{
	case 0: 
		mBehaviour = stand;
		break;
	case 1: 
		mBehaviour = walk;
		break;
	}

	// Behaviour override
	//mBehaviour = run;

}


BullEntityNode::~BullEntityNode()
{

}

void BullEntityNode::Update()
{
	EntityNode::Update();

	// Similar to cow
	// Walks around (but faster) and grazes
	// Will thrash if picked up and will run for longer when dropped

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

		float minPeriod = 3.0f;
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
		glm::vec3 dirVec = glm::vec3(
			-1.0f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (1.0f - (-1.0f))),
			0.0f,
			-1.0f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (1.0f - (-1.0f)))
		);

		mVelocity += 0.02f * glm::normalize(dirVec);
		mVelocity = 0.2f * glm::normalize(mVelocity);
		Rotate(mVelocity);
	}
	else if (mBehaviour == run)
	{
		glm::vec3 dirVec = glm::vec3(
			-1.0f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (1.0f - (-1.0f))),
			0.0f,
			-1.0f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (1.0f - (-1.0f)))
		);

		mVelocity += 0.3f * glm::normalize(dirVec);
		mVelocity = 0.6f * glm::normalize(mVelocity);
		Rotate(mVelocity);
	}

	//Thrashing will occur when picked up?
	//When dropped, need to manually set mNextTimer to a greater number than cow
}

FarmerEntityNode::FarmerEntityNode(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture /*= NULL*/)
	: EntityNode(name, geometry, material, texture)
	, mLastTimer(0.0f)
	, mNextTimer(0.0f) 
{

}

FarmerEntityNode::~FarmerEntityNode()
{

}

void FarmerEntityNode::Update()
{
	return;
	BaseNode* rootNode = this;
	
	while (rootNode->getName() != "ROOT")
	{
		rootNode = rootNode->getParentNode();
	}

	if (!rootNode)
		throw("Root Node could not be found from " + getName());

	PlayerNode* playerNode;
	
	for (BaseNode* m : rootNode->getChildNodes())
	{
		if (m->getName() == "PLAYER")
		{
			playerNode = dynamic_cast<PlayerNode*>(m);
			break;
		}
	}

	if (!playerNode) 
		throw("Player Node could not be found from " + getName());



	//glm::vec3 playerPos = playerNode
	

}

}


