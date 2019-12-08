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
#include "entity_node.h"


namespace game {

	// struct Object
	// An object on the map. Purely abstract - does not exist in the scene
	struct Object {
		glm::vec2 pos;
		int a = 0, b = 0;
		std::string type = "default";
		float rotation = 0;
	};

	// class MapGenerator
	// Handles map generation
	class MapGenerator
	{
	public:
		MapGenerator(SceneGraph* sceneGraph, int initWidth = 3, int initHeight = 3);
		~MapGenerator();
		void GenerateMap();

	private:
		// Scene graph containing all nodes to render
		SceneGraph* scene;

		PoissonGenerator::DefaultPRNG PRNG;
		void GenerateCluster(Object origin);

		//Generate points
		int gridWidth;
		int gridHeight;
		const int cellSize;
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
