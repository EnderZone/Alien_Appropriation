
#include "base_node.h"
#include "camera.h"

namespace game
{

BaseNode::BaseNode(std::string name)
	: mName(name)
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
	for (BaseNode* bn : getChildNodes())
	{
		bn->Update();
	}
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

}