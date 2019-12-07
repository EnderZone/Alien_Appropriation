#pragma once


#include <glm/glm.hpp>

#include <string.h>
#include <vector>

//#include "scene_graph.h"

//#include "game.h"

namespace game
{

	class Camera;
	class SceneGraph;

	class BaseNode {

	public:
		BaseNode(std::string name);
		~BaseNode();

		virtual void Draw(Camera* camera, glm::mat4 parentTransf = glm::mat4(1.0));
		virtual void Update();

		const std::string getName() const { return mName; }
		inline void SetName(std::string new_name) { mName = new_name; }

		inline void setParentNode(BaseNode* n) { mParentNode = n; }
		inline BaseNode* getParentNode() { return mParentNode; }

		template<class T> void addChildNode(T* n) { mChildNodes.push_back(n); n->setParentNode(this); }

		inline std::vector<BaseNode*> getChildNodes() { return mChildNodes; }
		void removeChildNode(std::string name);

		BaseNode* getRootNode();

		inline void setSceneGraph(SceneGraph* s) { mSceneGraph = s; }
		inline SceneGraph* getSceneGraph() { return mSceneGraph; }

		void deleteNodes(std::vector<std::string>& nodesToDelete);

	protected:
		std::string mName;

		BaseNode* mParentNode;
		std::vector<BaseNode*> mChildNodes;

		bool mDeleteNextTick;
		
	private:
		// Only the root node has a pointer to the scene graph
		SceneGraph* mSceneGraph;

		//Game* mGame;
	};


}