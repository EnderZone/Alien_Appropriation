#pragma once


#include <glm/glm.hpp>

#include <string.h>
#include <vector>

namespace game
{

	class Camera;

	class BaseNode {

	public:
		BaseNode(std::string name);
		~BaseNode();

		virtual void Draw(Camera* camera, glm::mat4 parentTransf = glm::mat4(1.0));
		virtual void Update();

		const std::string getName() const { return mName; }

		inline void setParentNode(BaseNode* n) { mParentNode = n; }
		inline BaseNode* getParentNode() { return mParentNode; }

		template<class T> void addChildNode(T* n) { mChildNodes.push_back(n); n->setParentNode(this); }

		inline std::vector<BaseNode*> getChildNodes() { return mChildNodes; }
		void removeChildNode(std::string name);

	protected:
		std::string mName;

		BaseNode* mParentNode;
		std::vector<BaseNode*> mChildNodes;
	};


}