

#include "projectile_node.h"


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

	/*// Check to see if this laser intersects any asteroids
	BaseNode* currentNode = this;
	while (currentNode->getName() != "ROOT")
		currentNode = currentNode->getParentNode();
	BaseNode* rootNode=currentNode;

	//Find the ray origin
	glm::vec3 rayOrigin = glm::vec3(0.0f);
	currentNode = getParentNode();

	if (SceneNode* node = dynamic_cast<SceneNode*>(currentNode))
	{
		glm::quat camInverseQuat = (((Camera*)(node->getParentNode()))->GetOrientation());
		camInverseQuat.x *= -1.0f;
		camInverseQuat.y *= -1.0f;
		camInverseQuat.z *= -1.0f;

		glm::vec3 rayDirection = glm::normalize(((Camera*)(node->getParentNode()))->GetForward());
		rayOrigin = node->GetPosition() * camInverseQuat + ((Camera*)(node->getParentNode()))->GetPosition();
		
		for (BaseNode* currentNode : rootNode->getChildNodes())
		{
			if (Asteroid* currentAsteroidNode = dynamic_cast<Asteroid*>(currentNode))
			{
				// Check if ray origin is inside asteroid
				glm::vec3 sphereOrigin = currentAsteroidNode->GetPosition();
				glm::vec3 sphereToRayOriginDirection = rayOrigin - sphereOrigin;
				if (glm::length(sphereToRayOriginDirection) < currentAsteroidNode->getRadius())
					currentNode->removeChildNode(currentAsteroidNode->getName());

				// If dot prod is more than 0, sphere is behind. can ignore
				if (glm::dot(sphereToRayOriginDirection, rayDirection) > 0)
					continue;

				glm::vec3 rayOriginAndSphereToRayOriginProjection = glm::dot(rayDirection, sphereToRayOriginDirection) * rayDirection;
				glm::vec3 shortedVectorFromSphereOriginToRay = rayOriginAndSphereToRayOriginProjection - sphereToRayOriginDirection;

				if(glm::length(shortedVectorFromSphereOriginToRay) < currentAsteroidNode->getRadius())
					rootNode->removeChildNode(currentAsteroidNode->getName());
			}
		}
	}
	return;*/
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


	// Get the player node
	BaseNode* rootNode = this;

	while (rootNode->getName() != "ROOT")
	{
		rootNode = rootNode->getParentNode();
	}

	if (!rootNode)
		throw("Root Node could not be found from " + getName());

	SceneNode* playerNode;

	for (BaseNode* m : rootNode->getChildNodes())
	{
		if (m->getName() == "PLAYER")
		{
			playerNode = reinterpret_cast<SceneNode*>(m);
			break;
		}
	}

	if (!playerNode)
		throw("Player Node could not be found from " + getName());


	// Comment this out to switch back to player pos
	Camera* cameraNode;
	for (BaseNode* m : playerNode->getChildNodes())
	{
		if (m->getName() == "CAMERA")
		{
			cameraNode = reinterpret_cast<Camera*>(m);
			break;
		}
	}
	glm::vec3 playerPos = cameraNode->GetPosition();
	//glm::vec3 playerPos = playerNode->GetPosition();

	glm::vec3 dirPlayer = playerPos - mPosition;
	Rotate(dirPlayer);

	mVelocity += 0.2f * dirPlayer;
	mVelocity = mMaxVelocity * glm::normalize(mVelocity);
}

}