#ifndef GAME_H_
#define GAME_H_

#include <exception>
#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "scene_graph.h"
#include "resource_manager.h"
#include "camera.h"
#include "projectile_node.h"
#include "entity_node.h"

namespace game {

    // Exception type for the game
    class GameException: public std::exception
    {
        private:
            std::string message_;
        public:
            GameException(std::string message) : message_(message) {};
            virtual const char* what() const throw() { return message_.c_str(); };
            virtual ~GameException() throw() {};
    };

    // Game application
    class Game {

        public:
            // Constructor and destructor
            Game(void);
            ~Game();
			// Call Init() before calling any other method
            void Init(void);
            // Set up resources for the game
            void SetupResources(void);
            // Set up initial scene
            void SetupScene(void);
            // Run the game: keep the application active
            void MainLoop(void);

        private:
            // GLFW window
            GLFWwindow* mWindow;

            // Scene graph containing all nodes to render
            SceneGraph* mSceneGraph;

            // Resources available to the game
            ResourceManager* mResourceManager;

            // Camera abstraction
            Camera* mCamera;

            // Flag to turn animation on/off
            bool mAnimating;

            // Methods to initialize the game
            void InitWindow(void);
            void InitView(void);
            void InitEventHandlers(void);

            // Methods to handle events
            static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
            static void ResizeCallback(GLFWwindow* window, int width, int height);

			// Create an instance of an object stored in the resource manager
			template<class T> T *CreateInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name = std::string(""))
			{

				Resource *geom = mResourceManager->GetResource(object_name);
				if (!geom) {
					throw(GameException(std::string("Could not find resource \"") + object_name + std::string("\"")));
				}

				Resource *mat = mResourceManager->GetResource(material_name);
				if (!mat) {
					throw(GameException(std::string("Could not find resource \"") + material_name + std::string("\"")));
				}

				Resource *tex = NULL;
				if (texture_name != "") {
					tex = mResourceManager->GetResource(texture_name);
					if (!tex) {
						throw(GameException(std::string("Could not find resource \"") + material_name + std::string("\"")));
					}
				}

				T *scn = mSceneGraph->CreateNode<T>(entity_name, geom, mat, tex);
				return scn;
			}

	}; // class Game

} // namespace game

#endif // GAME_H_
