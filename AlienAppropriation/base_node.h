#pragma once

#include <glm/glm.hpp>
#include <string.h>
#include <vector>



namespace game
{

	// class BaseNode
	// The most basic type of node. Contains functionality for existing within a hierarchy - parenting and children
	// Can do nothing on its own
	class BaseNode {

	protected:
		std::string mName;
		BaseNode* mParentNode;
		std::vector<BaseNode*> mChildNodes;
		std::vector<std::string> tags;


	public:
		BaseNode(std::string name);
		~BaseNode();

		virtual void update(double deltaTime);

		// Getters
		const std::string getName() const { return mName; }
		inline BaseNode* getParentNode() { return mParentNode; }
		inline std::vector<BaseNode*> getChildNodes() { return mChildNodes; }

		// Setters
		inline void setName(std::string new_name) { mName = new_name; }
		inline void setParentNode(BaseNode* n) { mParentNode = n; }

		// Children
		template<class T> void addChildNode(T* n) { mChildNodes.push_back(n);}
		void removeChildNode(std::string name);
		void removeChildNode(BaseNode* node);

		BaseNode* getRootNode();

		// Tags
		void addTag(std::string tag);
		void removeTag(std::string tag);
		bool hasTag(std::string tag);

	};


}