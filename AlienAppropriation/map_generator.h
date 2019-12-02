#pragma once
#ifndef MAP_GENERATOR_H_
#define MAP_GENERATOR_H_


#include <exception>
#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtx/vector_angle.hpp>

#define POISSON_PROGRESS_INDICATOR 1
#include "PoissonGenerator.h"


#include "scene_graph.h"
#include "resource_manager.h"


namespace game {
	// Exception type for the game
	class GameException2 : public std::exception
	{
	private:
		std::string message_;
	public:
		GameException2(std::string message) : message_(message) {};
		virtual const char* what() const throw() { return message_.c_str(); };
		virtual ~GameException2() throw() {};
	};

	struct Object {
		glm::vec2 pos;
		int a = 0, b = 0;
		std::string type = "default";
		float rotation = 0;
	};

	class MapGenerator
	{
	public:
		MapGenerator(SceneGraph* sceneGraph, ResourceManager* resourceManager);
		~MapGenerator();
		void GenerateMap();

	private:
		// Scene graph containing all nodes to render
		SceneGraph* scene;

		// Resources available to the game
		ResourceManager* resman;

		PoissonGenerator::DefaultPRNG PRNG;


		// Create an instance of an object stored in the resource manager
		SceneNode *CreateInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name = std::string(""), SceneNode* parent=nullptr);

		void GenerateCluster(Object origin);


		//Generate points
		int gridWidth;
		int gridHeight;
		int cellSize;
		int width;
		int height;
		int difficulty;

		float density;
		int cowCount;
		int enemyCount;

		std::vector<std::vector<std::vector<Object>>> grid;

	};

}

#endif
