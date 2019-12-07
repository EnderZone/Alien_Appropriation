
#include "base_node.h"
#include "camera.h"

namespace game
{

BaseNode::BaseNode(std::string name)
	: mName(name)
	, mDeleteNextTick(false)
{

}

BaseNode::~BaseNode()
{
	//do nothing
}

void BaseNode::Draw(Camera* camera, glm::mat4 parentTransf /*= glm::mat4(1.0)*/)
{
	for (BaseNode* bn : getChildNodes())
	{
		bn->Draw(camera, parentTransf);
	}
}

void BaseNode::Update()
{
	for (BaseNode* m : getChildNodes())
	{
		if (m->getName() == "CAMERA")
		{
			m->Update();
			break;
		}
	}

	for (BaseNode* bn : getChildNodes())
	{
		if (bn->getName() != "CAMERA")
		{
			bn->Update();
		}
	}

	std::vector<std::string> nodesToDelete;
	deleteNodes(nodesToDelete);
}


void BaseNode::removeChildNode(std::string name)
{
	int index = 0;
	for (BaseNode* n : getChildNodes())
	{
		if (n->getName() == name)
		{
			mChildNodes.erase(mChildNodes.begin() + index);
		}
		index++;
	}
}

BaseNode* BaseNode::getRootNode()
{
	// Get the root node
	BaseNode* rootNode = this;
	while (rootNode->getName() != "ROOT")
	{
		rootNode = rootNode->getParentNode();
	}

	if (!rootNode)
		throw("Root Node could not be found from " + getName());

	return rootNode;
}

void BaseNode::deleteNodes(std::vector<std::string>& nodesToDelete)
{
	// go through all nodes of root
	// iterate and find any to be deleted and add to vector
	// remove these nodes from hierarchy
	// delete these nodes

	if (mName == "ROOT")
	{
		for (BaseNode* n : getChildNodes())
		{
			n->deleteNodes(nodesToDelete);

			for (std::string nodeName : nodesToDelete)
			{
				removeChildNode(nodeName);
			}
		}
	}
	else
	{
		if (mDeleteNextTick)
			nodesToDelete.push_back(mName);
	}


}

}