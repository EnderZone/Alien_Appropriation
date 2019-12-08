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
#include "player_node.h"
#include "map_generator.h"

namespace game {
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

			MapGenerator* mMapGenerator;

            // Camera abstraction
            Camera* mCamera;

			SceneNode *skybox_;

            // Methods to initialize the game
            void InitWindow(void);
            void InitView(void);
            void InitEventHandlers(void);

            // Methods to handle events
            static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
            static void ResizeCallback(GLFWwindow* window, int width, int height);

	}; // class Game

} // namespace game

#endif // GAME_H_
