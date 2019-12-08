#ifndef CAMERA_H_
#define CAMERA_H_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#include <string>

#include "scene_node.h"


namespace game {
	
	class PlayerNode;

	enum Perspective {First, Third};

    // class Camera
	// The camera node contains a bit more functionality than one would expect
	// This is because camera node is instrinsically linked to the player - since the player is a child of the camera
    class Camera : public SceneNode {
		private:

			glm::vec3 mForward; // Initial forward vector
			glm::vec3 mSide; // Initial side vector
			glm::mat4 mViewMatrix; // View matrix
			glm::mat4 mProjectionMatrix; // Projection matrix


			glm::vec3 mVelocity;


			glm::vec3 playerForward;

			// Create view matrix from current camera parameters
			void SetupViewMatrix(void);

			Perspective mCameraPerspective;

        public:
            Camera(std::string name);
            ~Camera();
 
			// Dummy draw function, just to update parentTransF for any children
			virtual void draw(SceneNode *camera, glm::mat4 parentTransf = glm::mat4(1.0));
			virtual void update(double deltaTime);

			// Camera Perspective
			void SwitchCameraPerspective();

            // Get relative attributes of camera
            glm::vec3 GetForward(void) const;
            glm::vec3 GetSide(void) const;
            glm::vec3 GetUp(void) const;

			// Velocity variables
			inline glm::vec3 getVelocityRelative() { return mVelocity; }

			inline void setVelocity(glm::vec3 velocity) { mVelocity = velocity; }
			inline void addVelocity(glm::vec3 velocity) { mVelocity += velocity;  }

			inline glm::vec3 getVelocityRaw() {
				glm::vec3 current = mVelocity.x * glm::cross(playerForward, glm::vec3(0.0f, -1.0f, 0.0f));
				current += -mVelocity.z * playerForward;
				return current;
			};

            // Perform relative transformations of camera
            void Pitch(float angle);
            void Yaw(float angle);
            void Roll(float angle);

            // Set the view from camera parameters: initial position of camera,
            // point looking at, and up vector
            // Resets the current orientation and position of the camera
            void SetView(glm::vec3 position, glm::vec3 look_at, glm::vec3 up);
            // Set projection from frustum parameters: field-of-view,
            // near and far planes, and width and height of viewport
            void SetProjection(GLfloat fov, GLfloat near, GLfloat far, GLfloat w, GLfloat h);
            // Set all camera-related variables in shader program
            virtual void SetupShader(GLuint program, glm::mat4& parentTransf = glm::mat4(1.0));

			inline float GetHeight() { return mPosition.y; };



    }; // class Camera

} // namespace game

#endif // CAMERA_H_
