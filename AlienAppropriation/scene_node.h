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

namespace game {
	
	enum CollisionType { Point, Capsule, None };

	// class SceneNode
	// A node that exists within a scene. It has a 3D transform.
	// Additionaly, scene nodes can be drawn and can collide.
	class SceneNode : public BaseNode {

		protected:


			// 3D Transform
			glm::vec3 mPosition;
			glm::quat mOrientation;
			glm::vec3 mScale;

			// Collision
			glm::vec2 gridPosition;
			float radius;
			CollisionType collisionType;

			// drawing
			GLuint mArrayBuffer; // References to geometry: vertex and array buffers
			GLuint mElementArrayBuffer;
			GLenum mMode; // Type of geometry
			GLsizei mSize; // Number of primitives in geometry
			GLuint mMaterial; // Reference to shader program
			GLuint mTexture; // Reference to texture resource
			GLuint mEnvmap; // Reference to environment map


			// Quaternion helper function
			// Finds a quat such that q*start = dest
			// Source code from https://github.com/opengl-tutorials/ogl/blob/master/common/quaternion_utils.cpp
			glm::quat QuatBetweenVectors(glm::vec3 start, glm::vec3 dest);

		public:
			SceneNode(const std::string name);
			SceneNode(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture = NULL, const Resource *envmap = NULL);
			~SceneNode();

			// Draw the node relative to its parent according to scene parameters in 'camera'
			virtual void draw(SceneNode *camera, glm::mat4 parentTransf = glm::mat4(1.0));
			virtual void update(double deltaTime);

			// Transformations
			void translate(glm::vec3 trans);
			void rotate(glm::quat rot);
			void rotate(glm::vec3 direction);
			void scale(glm::vec3 scale);

			// Getters
			virtual glm::vec3 getPosition(void);
			glm::quat getOrientation(void) const;
			glm::vec3 getscale(void) const;
			inline glm::vec2 getGridPosition(void) { return gridPosition; }
			inline float getRadius(void) { return radius; }
			inline CollisionType getCollisionType(void) { return collisionType; }

			// OpenGL variables
			GLenum getMode(void) const;
			GLuint getArrayBuffer(void) const;
			GLuint getElementArrayBuffer(void) const;
			GLsizei getSize(void) const;
			GLuint getMaterial(void) const;
			GLuint getTexture(void) const;

			// Setters
			void setPosition(glm::vec3 position);
			void setOrientation(glm::quat orientation);
			void setScale(glm::vec3 scale);
			void setGridPosition(glm::vec3 pos);
			void setEnvMap(Resource *envmap);
			virtual void SetupShader(GLuint program, glm::mat4& parentTransf = glm::mat4(1.0));	// Set matrices that transform the node in a shader program


	}; // class SceneNode
} // namespace game

#endif // SCENE_NODE_H_
