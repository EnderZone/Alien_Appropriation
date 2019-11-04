#include <stdexcept>
#include <iostream>
#include <fstream>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "scene_graph.h"

namespace game {

SceneGraph::SceneGraph(void){

    mBackgroundColor = glm::vec3(0.0, 0.0, 0.0);

	mRootNode = new BaseNode("ROOT");
	
}


SceneGraph::~SceneGraph(){
}


void SceneGraph::SetBackgroundColor(glm::vec3 color){

    mBackgroundColor = color;
}


glm::vec3 SceneGraph::GetBackgroundColor(void) const {

    return mBackgroundColor;
}
 

SceneNode *SceneGraph::CreateNode(std::string node_name, Resource *geometry, Resource *material)
{
    // Create scene node with the specified resources
    SceneNode *scn = new SceneNode(node_name, geometry, material);

    // Add node to the scene
	mRootNode->addChildNode(scn);

    return scn;
}


void SceneGraph::AddNode(SceneNode *node)
{
    //node_.push_back(node);
	mRootNode->addChildNode(node);
}


game::BaseNode* SceneGraph::GetNode(std::string node_name, BaseNode* currentNode /*= nullptr*/) const 
{
	if (currentNode == nullptr)
		currentNode = mRootNode;

	if (currentNode->getName() == node_name)
		return currentNode;
	else
	{
		for (BaseNode* node : currentNode->getChildNodes())
		{
			if (GetNode(node_name, node) != nullptr)
				return GetNode(node_name, node);
		}
		return nullptr;
	}

    return nullptr;
}



void SceneGraph::Draw(Camera *camera)
{
    // Clear background
    glClearColor(mBackgroundColor[0], 
                 mBackgroundColor[1],
                 mBackgroundColor[2], 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mRootNode->Draw(camera);
}


void SceneGraph::Update(void)
{
	mRootNode->Update();
}

} // namespace game
