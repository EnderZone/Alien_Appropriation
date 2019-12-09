#ifndef SCENE_GRAPH_H_
#define SCENE_GRAPH_H_

#include <string>
#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include <stack>

#include "base_node.h"

#include "scene_node.h"
#include "camera.h"
#include "player_node.h"
#include "resource_manager.h"
#include "projectile_node.h"
#include "entity_node.h"

namespace game {

	// Exception type for the game
	class GameException : public std::exception
	{
	private:
		std::string message_;
	public:
		GameException(std::string message) : message_(message) {};
		virtual const char* what() const throw() { return message_.c_str(); };
		virtual ~GameException() throw() {};
	};

    // class SceneGraph
	// The Scene Graph contains all nodes within the scene.
	// It is responsible for managing nodes: creating, updating, and deleting
	// It also takes care of any high-level functionality that affects all nodes, such as collision

	// Nodes are stored in a vector - the hierarchy is used for drawing

    class SceneGraph {

        private:
            // Background color
            glm::vec3 mBackgroundColor;

			// Reference to important nodes
			static BaseNode* mRootNode;
			static PlayerNode* mPlayerNode;
			Camera* mCameraNode;

			static std::vector<std::vector<std::vector<SceneNode*>>> nodes;




        public:
            // Constructor and destructor
            SceneGraph(Camera* camera);
            ~SceneGraph();

            // Background color
            void SetBackgroundColor(glm::vec3 color);
            glm::vec3 GetBackgroundColor(void) const;
            
			// Basic functionality
			void draw(Camera *camera);
			bool update(double deltaTime);
			bool checkCollisionWithPlayer(SceneNode *object);
			bool checkCollisionBetweenObjs(SceneNode *bomb, SceneNode *target);

			// Getters
			inline static BaseNode* getRootNode() { return mRootNode; }
			inline static PlayerNode* getPlayerNode() { return mPlayerNode; }
			inline Camera* getCameraNode() { return mCameraNode; }

			// Setters
			inline void setPlayerNode(PlayerNode* player) { mPlayerNode = player; }

			// Hierarchy Management
			static void addNode(SceneNode *node, BaseNode *parent = nullptr) 
			{
				if (parent) {
					node->setParentNode(parent);
					parent->addChildNode(node);
				}
				else if (node->getParentNode() == node || parent == nullptr) {
					node->setParentNode(mRootNode);
					mRootNode->addChildNode(node);
				}
				int x = (int)floor(node->getPosition().x / 20.0f);
				int y = (int)floor(node->getPosition().y / 20.0f);
				x = glm::clamp(x, 0, 14);
				y = glm::clamp(y, 0, 14);
				node->setGridPosition(x, y);

				nodes.at(x).at(y).push_back(node);
			}

			void deleteNode(BaseNode *node);
			void deleteNode(std::string name);
			BaseNode* getNode(std::string node_name);


			// Node Creation
			template<class T>
			static T* CreateNode(std::string node_name, Resource *geometry, Resource *material, Resource *texture = NULL, BaseNode* parent = nullptr)
			{
				// Create scene node with the specified resources
				T* scn = new T(node_name, geometry, material, texture);

				addNode(scn, parent);

				return scn;
			}

			template<class T>
			static T* CreateProjectileNode(std::string node_name, Resource *geometry, Resource *material, Resource *texture, float lifespan, glm::vec3 initialPos, glm::vec3 initialVelocityVec)
			{
				// Create scene node with the specified resources
				T* scn = new T(node_name, geometry, material, lifespan, initialPos, initialVelocityVec, texture);

				// Add node to the scene
				mRootNode->addChildNode(scn);
				scn->setParentNode(mRootNode);
				int x = glm::clamp((int)floor(initialPos.x / 20.0f), 0, 14);
				int y = glm::clamp((int)floor(initialPos.y / 20.0f), 0, 14);

				nodes.at(x).at(y).push_back(scn);
				scn->setGridPosition(x, y);

				return scn;
			}

			// Node Creation + Resource Fetching
			template<class T> static T *CreateInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name = std::string(""), BaseNode *parent=nullptr)
			{
				
				Resource *geom =  ResourceManager::getResource(object_name);
				if (!geom) {
					throw(GameException(std::string("Could not find resource \"") + object_name + std::string("\"")));
				}

				Resource *mat = ResourceManager::getResource(material_name);
				if (!mat) {
					throw(GameException(std::string("Could not find resource \"") + material_name + std::string("\"")));
				}

				Resource *tex = NULL;
				if (texture_name != "") {
					tex = ResourceManager::getResource(texture_name);
					if (!tex) {
						throw(GameException(std::string("Could not find resource \"") + material_name + std::string("\"")));
					}
				}

				T *scn = CreateNode<T>(entity_name, geom, mat, tex, parent);
				return scn;
			}

			template<class T> static T *CreateProjectileInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name, float lifespan, glm::vec3 initialPos, glm::vec3 initialVelocityVec)
			{
				Resource *geom = ResourceManager::getResource(object_name);
				if (!geom) {
					throw(("Could not find resource \"") + object_name + "\"");
				}

				Resource *mat = ResourceManager::getResource(material_name);
				if (!mat) {
					throw("Could not find resource \"" + material_name + "\"");
				}

				Resource *tex = NULL;
				if (texture_name != "") {
					tex = ResourceManager::getResource(texture_name);
					if (!tex) {
						throw("Could not find resource \"" + material_name + "\"");
					}
			}

				T *scn = CreateProjectileNode<T>(entity_name, geom, mat, tex, lifespan, initialPos, initialVelocityVec);
				return scn;
			}





    }; // class SceneGraph

} // namespace game

#endif // SCENE_GRAPH_H_
