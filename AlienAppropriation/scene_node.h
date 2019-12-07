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


	enum CollisionType { Point, Capsule };


	// Class that manages one object in a scene 
	class SceneNode : public BaseNode {

	public:
		// Create scene node from given resources
		SceneNode(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture = NULL);

		// Destructor
		~SceneNode();

		// Get node attributes
		virtual glm::vec3 GetPosition(void);
		glm::quat GetOrientation(void) const;
		glm::vec3 GetScale(void) const;
		inline glm::vec2 GetGridPosition(void) { return gridPosition; }
		inline float GetRadius(void) { return radius; }
		inline CollisionType GetCollisionType(void) { return collisionType; }

		// Set node attributes
		void SetPosition(glm::vec3 position);
		void SetOrientation(glm::quat orientation);
		void SetScale(glm::vec3 scale);
		void SetGridPosition(glm::vec3 pos);

		// Perform transformations on node
		void Translate(glm::vec3 trans);
		void Rotate(glm::quat rot);
		void Rotate(glm::vec3 direction);
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
		GLuint GetTexture(void) const;

	protected:

		glm::vec3 mPosition; // Position of node
		glm::quat mOrientation; // Orientation of node
		glm::vec3 mScale; // Scale of node

		GLuint mArrayBuffer; // References to geometry: vertex and array buffers
		GLuint mElementArrayBuffer;
		GLenum mMode; // Type of geometry
		GLsizei mSize; // Number of primitives in geometry
		GLuint mMaterial; // Reference to shader program
		GLuint mTexture; // Reference to texture resource

		// Set matrices that transform the node in a shader program
		void SetupShader(GLuint program, glm::mat4& parentTransformation = glm::mat4(1.0));

		// Quaternion helper fn
		// Finds a quat such that q*start = dest
		// Source code from https://github.com/opengl-tutorials/ogl/blob/master/common/quaternion_utils.cpp
		glm::quat QuatBetweenVectors(glm::vec3 start, glm::vec3 dest);


		glm::vec2 gridPosition;
		//bool collidable;
		float radius;
		CollisionType collisionType;


	}; // class SceneNode
} // namespace game

#endif // SCENE_NODE_H_
