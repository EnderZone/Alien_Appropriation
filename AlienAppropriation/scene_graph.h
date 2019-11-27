#ifndef SCENE_GRAPH_H_
#define SCENE_GRAPH_H_

#include <string>
#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "base_node.h"

#include "scene_node.h"
#include "resource.h"
#include "camera.h"
#include "player_node.h"


// Size of the texture that we will draw
#define FRAME_BUFFER_WIDTH 1024
#define FRAME_BUFFER_HEIGHT 768

namespace game {

    // Class that manages all the objects in a scene
    class SceneGraph {

        private:
            // Background color
            glm::vec3 mBackgroundColor;

			// Root Node
			BaseNode* mRootNode;

			// Player Node
			BaseNode* mPlayerNode;


			// Frame buffer for drawing to texture
			GLuint mFrameBuffer;
			// Quad vertex array for drawing from texture
			GLuint mQuadArrayBuffer;
			// Render targets
			GLuint mTexture;
			GLuint mDepthBuffer;

        public:
            // Constructor and destructor
            SceneGraph(void);
            ~SceneGraph();

            // Background color
            void SetBackgroundColor(glm::vec3 color);
            glm::vec3 GetBackgroundColor(void) const;
            
            // Create a scene node from the specified resources
            SceneNode *CreateNode(std::string node_name, Resource *geometry, Resource *material, Resource *texture = NULL);
			SceneNode* CreatePlayerNode(std::string node_name, Resource *geometry, Resource *material, Resource *texture = NULL, BaseNode* camera = NULL);
			// Add an already-created node
            void AddNode(SceneNode *node);
            // Find a scene node with a specific name
            BaseNode* GetNode(std::string node_name, BaseNode* currentNode = nullptr) const;
			
			// Keep pointer to root and player for easier access
			inline BaseNode* getRootNode() { return mRootNode; }
			inline BaseNode* getPlayerNode() { return mPlayerNode; }

            // Draw the entire scene
            void Draw(Camera *camera);

            // Update entire scene
            void Update(void);


			// Drawing from/to a texture
			// Setup the texture
			void SetupDrawToTexture(void);
			// Draw the scene into a texture
			void DrawToTexture(Camera *camera);
			// Process and draw the texture on the screen
			void DisplayTexture(GLuint program);
			// Save texture to a file in ppm format
			void SaveTexture(char *filename);

    }; // class SceneGraph

} // namespace game

#endif // SCENE_GRAPH_H_
