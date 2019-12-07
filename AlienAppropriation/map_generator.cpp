#include "map_generator.h"

namespace game {



	MapGenerator::MapGenerator(SceneGraph* sceneGraph, ResourceManager* resourceManager)
	{
		scene = sceneGraph;
		resman = resourceManager;


		//Generate points
		gridWidth = 20;
		gridHeight = 20;
		cellSize = 15;
		width = gridWidth * cellSize;
		height = gridHeight * cellSize;
		difficulty = 1;

		density = 1;
		cowCount;
		enemyCount;
		for (int i = 0; i < gridWidth; i++) {
			std::vector<std::vector<Object>> column(gridHeight, std::vector<Object>());	
			grid.push_back(column);
		}
	}


	MapGenerator::~MapGenerator()
	{
	}

	void MapGenerator::GenerateMap()
	{

		//Begin by creating a ground plane
		for (int i = 0; i < width/100; i++) {
			for (int j = 0; j < height/100; j++) {
				SceneNode* ground = CreateInstance("Ground" + std::to_string(i) + std::to_string(j), "GridMesh", "litTextureMaterial", "groundTexture");
				ground->Translate(glm::vec3(i * 100, 0, j * 100));
			}
		}

		// Generate random points
		const auto Points = PoissonGenerator::generatePoissonPoints((gridWidth+1) * (gridHeight+1) * density, PRNG,50,false, 1/(density * glm::min(gridWidth, gridHeight)));
		
		// Sort the random points into grid cells based off thier position
		for (auto p : Points) {
			Object point;
			point.pos = glm::vec2(p.x * width, p.y * height);
			point.a = floor(point.pos.x / cellSize); 
			point.b = floor(point.pos.y / cellSize);
			point.type = "hay";
			int r = rand() % 100;
			if (r < 15) {
				point.type = "originPoint";
			}
			grid.at(point.a).at(point.b).push_back(point);
		}
		
		// Generate tight clusters of objects around certain points 
		for (int x = 0; x < gridWidth; x++) {
			for (int y = 0; y < gridHeight; y++) {
				for (int i = 0; i <grid.at(x).at(y).size(); i++) {
					if (grid.at(x).at(y).at(i).type == "originPoint") {
						GenerateCluster(grid.at(x).at(y).at(i));
					}
				}
			}
		}
		
		// Create objects at each point
		for (int x = 0; x < gridWidth; x++) {
			for (int y = 0; y < gridHeight; y++) {
				for (auto o : grid.at(x).at(y)) {
					if (!(o.type == "default" || o.type == "originPoint")) {
						SceneNode* obj = CreateInstance(o.type + std::to_string(x) + std::to_string(y), o.type + "Mesh", "litTextureMaterial", o.type + "Texture");
						obj->Translate(glm::vec3(o.pos.x, 0, o.pos.y));

						if (o.type == "hay") {
							obj->Rotate(glm::angleAxis(glm::half_pi<float>(), glm::vec3(0, 0, 1)));
							//obj->Rotate(glm::angleAxis((rand()%360) * (glm::pi<float>() / 180), glm::vec3(-1, 0, 0)));
							obj->Translate(glm::vec3(0, 0.5, 0));

						}
						if (o.type == "tree") {
							obj->Scale(glm::vec3(1.25f + rand() % 5 / 10.0f));
						}
						if (o.type == "barn") {
							obj->Rotate(glm::angleAxis(glm::radians(o.rotation), glm::vec3(0, 1, 0)));
							obj->Scale(glm::vec3(1.3f + rand() % 80 / 100.0f, 1.3f + rand() % 80 / 100.0f, 1.3f + rand() % 80 / 100.0f));
						}
					}
			/*		if (o.type == "originPoint") {
						SceneNode* tree = CreateInstance("Test", "treeMesh", "texturedMaterial", "barnTexture", ground);
						tree->Translate(glm::vec3(o.pos.x, 0, o.pos.y));
					}*/

				}
			}
		}



	}



	void MapGenerator::GenerateCluster(Object origin)
	{
		// Generate a tight cluster of objects around an origin point
		bool isBarnCluster = (rand() % 100) < 20;
		// The objects we generate are either trees or houses/barns

		//erase any points in adjacent cells to avoid overlap
		float radius = (isBarnCluster) ? cellSize : (1 + (rand() % 5) / 5.0f) * cellSize;
		for (int x = -1; x < 1; x++) {
			for (int y = -1; y < 1; y++) {
				// if the origin is on the border of the map, do not look for points outside the map
				if (origin.a + x < 0 || origin.a + x > gridWidth - 1 || origin.b + y < 0 || origin.b + y > gridHeight - 1) continue;
				// look in adjacent grid cells and ignore points within radius
				for (auto point : grid.at(origin.a + x).at(origin.b + y)) {
					if (glm::distance(origin.pos, point.pos) < radius) {
						point.type = "default";
					}
				}
			}
		}

		// Now that we've cleared some space, generate the cluster of objects
		int n;
		n = (isBarnCluster) ? rand() % 6 + 1 : rand() % 30 + 10;
		const auto Points = PoissonGenerator::generatePoissonPoints(n, PRNG, 70);
		for (auto p : Points) {
			Object point;
			point.pos = origin.pos +  glm::vec2(p.x * radius, p.y * radius) - radius/2.0f; //position the randomly generated point around the origin
			point.a = floor(point.pos.x / cellSize);
			point.b = floor(point.pos.y / cellSize);
			if (point.a > gridWidth - 1 || point.a < 0 || point.b > gridHeight - 1 || point.b < 0) continue;

			if (isBarnCluster) {
				point.type = "barn";
				switch (rand() % 3) {
				case 0: point.rotation = 0;  break;
				case 1: point.rotation = 90;  break;
				case 2: point.rotation = glm::orientedAngle(glm::normalize(origin.pos), glm::normalize(point.pos - origin.pos));  break;
				}
			}
			else {
				point.type = "tree";
			}
			grid.at(point.a).at(point.b).push_back(point);
		}
		
	}



	SceneNode* MapGenerator::CreateInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name, SceneNode* parent) {

		Resource *geom = resman->GetResource(object_name);
		if (!geom) {
			throw(GameException2(std::string("Could not find resource \"") + object_name + std::string("\"")));
		}

		Resource *mat = resman->GetResource(material_name);
		if (!mat) {
			throw(GameException2(std::string("Could not find resource \"") + material_name + std::string("\"")));
		}

		Resource *tex = NULL;
		if (texture_name != "") {
			tex = resman->GetResource(texture_name);
			if (!tex) {
				throw(GameException2(std::string("Could not find resource \"") + material_name + std::string("\"")));
			}
		}

		EntityNode *scn = scene->CreateNode<EntityNode>(entity_name, geom, mat, tex, parent);
		return scn;
	}


}

