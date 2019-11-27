#ifndef SCENE_NODE_H_
#define SCENE_NODE_H_

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#include "base_node.h"

#include "resource.h"
#include "camera.h"

namespace game {

    // Class that manages one object in a scene 
    class SceneNode : public BaseNode{

        public:
            // Create scene node from given resources
            SceneNode(const std::string name, const Resource *geometry, const Resource *material);

            // Destructor
            ~SceneNode();
            
            // Get node attributes
            glm::vec3 GetPosition(void) const;
            glm::quat GetOrientation(void) const;
            glm::vec3 GetScale(void) const;

            // Set node attributes
            void SetPosition(glm::vec3 position);
            void SetOrientation(glm::quat orientation);
            void SetScale(glm::vec3 scale);
            
            // Perform transformations on node
            void Translate(glm::vec3 trans);
            void Rotate(glm::quat rot);
            void Scale(glm::vec3 scale);

            // Draw the node according to scene parameters in 'camera'
            // variable
            virtual void Draw(Camera *camera, glm::mat4 parentTransf = glm::mat4(1.0));

            // Update the node
            virtual void Update(void);

            // OpenGL variables
            GLenum GetMode(void) const;
            GLuint GetArrayBuffer(void) const;
            GLuint GetElementArrayBuffer(void) const;
            GLsizei GetSize(void) const;
            GLuint GetMaterial(void) const;

        protected:
            GLuint mArrayBuffer; // References to geometry: vertex and array buffers
            GLuint mElementArrayBuffer;
            GLenum mMode; // Type of geometry
            GLsizei mSize; // Number of primitives in geometry
            GLuint mMaterial; // Reference to shader program
            glm::vec3 mPosition; // Position of node
            glm::quat mOrientation; // Orientation of node
            glm::vec3 mScale; // Scale of node

            // Set matrices that transform the node in a shader program
            virtual void SetupShader(GLuint program, glm::mat4& parentTransformation = glm::mat4(1.0));

    }; // class SceneNode

} // namespace game

#endif // SCENE_NODE_H_
