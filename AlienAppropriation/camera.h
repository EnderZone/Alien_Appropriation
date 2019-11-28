#ifndef CAMERA_H_
#define CAMERA_H_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#include <string>

#include "base_node.h"

namespace game {

    // Abstraction of a camera
    class Camera : public BaseNode{

        public:
            Camera(std::string name);
            ~Camera();
 
            // Get global camera attributes
            glm::vec3 GetPosition(void) const;
            glm::quat GetOrientation(void) const;

            // Set global camera attributes
            void SetPosition(glm::vec3 position);
            void SetOrientation(glm::quat orientation);
            
            // Perform global transformations of camera
            void Translate(glm::vec3 trans);
            void Rotate(glm::quat rot);


			// Dummy Draw function, just to update parentTransF for any children
			virtual void Draw(Camera *camera, glm::mat4 parentTransf = glm::mat4(1.0));
			virtual void Update();

			// Camera Perspective
			void SwitchCameraPerspective();

            // Get relative attributes of camera
            glm::vec3 GetForward(void) const;
            glm::vec3 GetSide(void) const;
            glm::vec3 GetUp(void) const;

			// Velocity variables
			inline void setVelocityForward(float v) { mVelocityZ = v; }
			inline float getVelocityForward() { return mVelocityZ; }
			
			inline void setVelocitySide(float v) { mVelocityX = v; }
			inline float getVelocitySide() { return mVelocityX; }


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
            void SetupShader(GLuint program);

        private:
            glm::vec3 mPosition; // Position of camera
            glm::quat mOrientation; // Orientation of camera
            glm::vec3 mForward; // Initial forward vector
            glm::vec3 mSide; // Initial side vector
            glm::mat4 mViewMatrix; // View matrix
            glm::mat4 mProjectionMatrix; // Projection matrix

			float mVelocityZ; //Velocity (only travels forward)
			float mVelocityX; //Velocity (only travels forward)


            // Create view matrix from current camera parameters
            void SetupViewMatrix(void);

			int mCameraPerspective;

    }; // class Camera

} // namespace game

#endif // CAMERA_H_
