#include <stdexcept>
#include <iostream>
#include <fstream>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "scene_graph.h"

#include "scene_node.h"

namespace game {

BaseNode* SceneGraph::mRootNode = nullptr;
PlayerNode* SceneGraph::mPlayerNode = nullptr;
std::vector<SceneNode*> SceneGraph::nodes;

SceneGraph::SceneGraph(Camera* camera) {

    mBackgroundColor = glm::vec3(0.0, 0.0, 0.0);

	mRootNode = new BaseNode("ROOT");
	addNode(camera);
	mCameraNode = camera;
}


//BaseNode* SceneGraph::mRootNode = new BaseNode("ROOT");
//std::vector<SceneNode*> SceneGraph::nodes;

SceneGraph::~SceneGraph(){
}


void SceneGraph::SetBackgroundColor(glm::vec3 color){

    mBackgroundColor = color;
}


glm::vec3 SceneGraph::GetBackgroundColor(void) const {

    return mBackgroundColor;
}


void SceneGraph::deleteNode(BaseNode * node)
{
	node->getParentNode()->removeChildNode(node);
	for (BaseNode* child : node->getChildNodes()) {
		child->setParentNode(nullptr);
		child->addTag("delete");
	}
}

void SceneGraph::deleteNode(std::string name)
{
	for (BaseNode* n : nodes)
	{
		if (name.compare(n->getName()) == 0)
		{
			deleteNode(n);
			return;
		}
	}

}


game::BaseNode* SceneGraph::getNode(std::string node_name) 
{
	for (BaseNode* n : nodes)
	{
		if (node_name.compare(n->getName()) == 0)
		{
			return n;
		}
	}

	return nullptr;
}



void SceneGraph::draw(Camera *camera)
{
    // Clear background
    glClearColor(mBackgroundColor[0], 
                 mBackgroundColor[1],
                 mBackgroundColor[2], 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (BaseNode* bn : mRootNode->getChildNodes())
	{
		dynamic_cast<SceneNode*>(bn)->draw(camera);
	}
}

// Check for collision
// If return true, then the object will be deleted (used for projectiles, cows)
bool SceneGraph::checkCollision(SceneNode * object)
{

	// Check if any objects below the player can be sucked up
	if (object->hasTag("canPickUp")) {
		if (mPlayerNode->isTractorBeamActive()) {
			float dist = glm::distance(glm::vec2(mPlayerNode->getPosition().x, mPlayerNode->getPosition().z), glm::vec2(object->getPosition().x, object->getPosition().z));
			float height = mPlayerNode->getPosition().y - object->getPosition().y;

			if (height > 0) {
				if (dist < height / 4 + 0.25) {
					dynamic_cast<EntityNode*>(object)->rise(glm::normalize(mPlayerNode->getPosition() - object->getPosition()));
				}
			}

		}
	}

	

	// Check for any other collision
	if (object->getCollisionType() == Point) {
		if ((glm::distance(object->getPosition(), mPlayerNode->getPosition())) < object->getRadius() + mPlayerNode->getRadius()) {
			
			
			// Check if any objects can be collected
			if (object->hasTag("canCollect")) {
				if (mPlayerNode->isTractorBeamActive() && (glm::distance(object->getPosition(), mPlayerNode->getPosition())) < object->getRadius() + mPlayerNode->getRadius()) {
					object->addTag("delete");
					if (object->hasTag("bull")) {
						mPlayerNode->takeDamage(BULL);
					}
					mPlayerNode->addCollected( object->hasTag("cow") ? "cow" : "hay" );
					return true;
				}
			}
			
			
			
			ProjectileNode* proj = dynamic_cast<ProjectileNode*> (object);
			if (proj) {
				proj->addTag("delete");
				if (!mPlayerNode->isShieldActive()) {
					mPlayerNode->takeDamage(MISSILE);
				}
				return true;
			}
			else {
				mCameraNode->setVelocity(glm::vec3(0));
			}

		}
	}

	return false;
}


void SceneGraph::update(double deltaTime)
{

	//consider moving update from the nodes to here
	mRootNode->update(deltaTime);

	mPlayerNode->setGridPosition(mPlayerNode->getPosition());
	for (int i = 0; i < nodes.size(); i++) {
		SceneNode* currentNode = nodes.at(i);
		if (currentNode->hasTag("delete")) {
			deleteNode(currentNode);
			nodes.erase(nodes.begin() + i);
			i--;
			continue;
			//delete currentNode;
		}
		if (currentNode->getName() == "camera" || currentNode->getName() == "player" || currentNode->hasTag("ignore")) continue;

		// Only check collision with objects in the adjacent grids
		// With this implementation, we're not actually making any savings, since we need to compare the grid positions
		//glm::vec2 gridDifference = (mPlayerNode->getGridPosition() - currentNode->getGridPosition());
		//if (glm::length(gridDifference) < 2.0f) {
		if (mPlayerNode->getGridPosition() == currentNode->getGridPosition()){
			checkCollision(currentNode);
		}
	}
}

} // namespace game
