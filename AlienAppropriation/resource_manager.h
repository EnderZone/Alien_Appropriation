#ifndef RESOURCE_MANAGER_H_
#define RESOURCE_MANAGER_H_

#include <string>
#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtx/rotate_vector.hpp>

#include "resource.h"

// Default extensions for different shader source files
#define VERTEX_PROGRAM_EXTENSION "_vp.glsl"
#define FRAGMENT_PROGRAM_EXTENSION "_fp.glsl"
#define GEOMETRY_PROGRAM_EXTENSION "_gp.glsl"

namespace game {

    // Class that manages all resources
    class ResourceManager {

        public:
            // Constructor and destructor
            ResourceManager(void);
            ~ResourceManager();
            // Add a resource that was already loaded and allocated to memory
            void AddResource(ResourceType type, const std::string name, GLuint resource, GLsizei size);
            void AddResource(ResourceType type, const std::string name, GLuint array_buffer, GLuint element_array_buffer, GLsizei size);
            // Load a resource from a file, according to the specified type
            void LoadResource(ResourceType type, const std::string name, const char *filename);
            // Get the resource with the specified name
            static Resource *getResource(const std::string name);

            // Methods to create specific resources
            // Create the geometry for a torus and add it to the list of resources
            void CreateTorus(std::string object_name, float loop_radius = 0.6, float circle_radius = 0.2, int num_loop_samples = 90, int num_circle_samples = 30);
            // Create the geometry for a sphere
            void CreateSphere(std::string object_name, float radius = 0.6, int num_samples_theta = 90, int num_samples_phi = 45);
			// Create the geometry for a cylinder
			void CreateCylinder(std::string object_name, float radius = 0.6, int resolution = 30);
			// Create the geometry for a cone
			void CreateCone(std::string object_name, float radius = 0.6, int resolution = 30);
			// Create the geometry of a plane using two triangles
			void CreateSquare(std::string object_name, float width = 1.0, glm::vec3 color = glm::vec3(1.0f));
			// Create the geometry of a plane using a grid
			void CreateGrid(std::string object_name, float heightVariance= 0, int width = 11, int height = 11, float tileSize = 10.0);
			// Create particles distributed over a sphere
			void CreateSphereParticles(std::string object_name, int num_particles = 20000);
			void CreateTorusParticles(std::string object_name, int num_particles, float loop_radius, float circle_radius);
			void CreateCube(std::string object_name);

	private:
            // List storing all resources
            static std::vector<Resource*> mResource; 
 
            // Methods to load specific types of resources
            // Load shaders programs
            void LoadMaterial(const std::string name, const char *prefix);
            // Load a text file into memory (could be source code)
			std::string LoadTextFile(const char *filename);
			// Load a texture from an image file: png, jpg, etc.
			void LoadTexture(const std::string name, const char *filename);
			// Loads a mesh in obj format
			void LoadMesh(const std::string name, const char *filename);
			void LoadCubeMap(const std::string name, const char *filename);

    }; // class ResourceManager

} // namespace game

#endif // RESOURCE_MANAGER_H_
