#include "base_node.h"
#include "camera.h"

namespace game
{

BaseNode::BaseNode(std::string name) : mName(name)
{
}

BaseNode::~BaseNode()
{
}

void BaseNode::update(double deltaTime)
{
	for (BaseNode* n : getChildNodes())	n->update(deltaTime);
}

void BaseNode::removeChildNode(std::string name)
{
	int index = 0;
	for (BaseNode* n : getChildNodes())
	{
		if (n->getName() == name)
		{
			mChildNodes.erase(mChildNodes.begin() + index);
			n->setParentNode(nullptr);
			continue;
		}
		index++;
	}
}

void BaseNode::removeChildNode(BaseNode * node)
{
	int index = 0;
	for (BaseNode* n : getChildNodes())
	{
		if (n == node)
		{
			mChildNodes.erase(mChildNodes.begin() + index);
			n->setParentNode(nullptr);
			continue;
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

void BaseNode::addTag(std::string tag)
{
	tags.push_back(tag);
}

void BaseNode::removeTag(std::string tag)
{
	for (int i = 0; i < tags.size(); i++) {
		if (tag.compare(tags.at(i)) == 0) {
			tags.erase(tags.begin() + i);
			return;
		}
	}
}

bool BaseNode::hasTag(std::string tag)
{
	for (std::string t : tags) {
		if (t.compare(tag) == 0) {
			return true;
		}
	}
	return false;
}
}