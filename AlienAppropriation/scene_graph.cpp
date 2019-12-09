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
std::vector<std::vector<std::vector<SceneNode*>>> SceneGraph::nodes(15, std::vector<std::vector<SceneNode*>>(15, std::vector<SceneNode*>()));

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
	for (std::vector<std::vector<SceneNode*>> column : nodes) {
		for (std::vector<SceneNode*> cell : column) {
			for (BaseNode* n : cell)
			{
				if (name.compare(n->getName()) == 0)
				{
					deleteNode(n);
					return;
				}
			}
		}
	}

}


game::BaseNode* SceneGraph::getNode(std::string node_name) 
{
	for (std::vector<std::vector<SceneNode*>> column : nodes) {
		for (std::vector<SceneNode*> cell : column) {
			for (BaseNode* n : cell)
			{
				if (node_name.compare(n->getName()) == 0)
				{
					return n;
				}
			}
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
bool SceneGraph::checkCollisionWithPlayer(SceneNode * object)
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
				else {
					mPlayerNode->addEnergy(-25.0f);
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

bool SceneGraph::checkCollisionBetweenObjs(SceneNode * bomb, SceneNode * target)
{
	if ((glm::distance(bomb->getPosition(), target->getPosition())) < bomb->getRadius() + target->getRadius()) {
		target->addTag("delete");
	}
	return false;
}


void SceneGraph::update(double deltaTime)
{
	//We iterate through all the nodes twice
	// Once to update them
	mRootNode->update(deltaTime);

	mPlayerNode->setGridPosition(mPlayerNode->getPosition());

	// Twice to delete any nodes, plus check collision
	for (int x = 0; x < nodes.size(); x++) {
		for (int y = 0; y < nodes.at(x).size(); y++) {
			std::vector<SceneNode*>& cell = nodes.at(x).at(y);
			for (int i = 0; i < cell.size(); i++) {
				SceneNode* currentNode = cell.at(i);

				// delete nodes
				if (currentNode->hasTag("delete")) {
					deleteNode(currentNode);
					cell.erase(cell.begin() + i);
					i--;
					continue;
				}

				// ignore player/camera nodes
				if (currentNode->getName() == "camera" || currentNode->getName() == "player" || currentNode->hasTag("ignore")) continue;

				// update grid location
				int newX = floor(currentNode->getPosition().x / 20);
				int newY = floor(currentNode->getPosition().z / 20);
				newX = glm::clamp(newX, 0, 14);
				newY = glm::clamp(newY, 0, 14);

				if (newX != x || newY != y) {
					cell.erase(cell.begin() + i);
					i--;
					nodes.at(newX).at(newY).push_back(currentNode);
					currentNode->setGridPosition(newX, newY);
					continue;
				}


				// check for collision with the player if the player is in the same grid OR the node is a projectile
				if (mPlayerNode->getGridPosition() == glm::vec2(x,y) || currentNode->hasTag("projectile")) {
					checkCollisionWithPlayer(currentNode);
				}

				// check collision between hay bombs and cannons
				// this is where the grid cells structure saves us time

				if (currentNode->hasTag("bomb")) {
					for (SceneNode* object : cell) {
						if (object->hasTag("bombable")) {
							checkCollisionBetweenObjs(currentNode, object);
						}
					}
				}
			}
		}
	}
}

} // namespace game
