#include <iostream>
#include <time.h>
#include <sstream>

#include "game.h"
#include "bin/path_config.h"
#include "entity_game_nodes.h"

namespace game {

// Some configuration constants
// They are written here as global variables, but ideally they should be loaded from a configuration file

// Main window settings
const std::string window_title_g = "Alien Appropriation - WIP";
const unsigned int window_width_g = 800;
const unsigned int window_height_g = 600;
const bool window_full_screen_g = false;

// Viewport and camera settings
float camera_near_clip_distance_g = 0.01;
float camera_far_clip_distance_g = 1000.0;
float camera_fov_g = 20.0; // Field-of-view of camera
const glm::vec3 viewport_background_color_g(0.2, 0.2, 0.7);
glm::vec3 camera_position_g(100.0, 15.0, 100.0);
glm::vec3 camera_look_at_g(100.0, 15.0, 50.0);
glm::vec3 camera_up_g(0.0, 1.0, 0.0);

// Materials
const std::string shader_directory = SHADER_DIRECTORY;
const std::string asset_directory = ASSET_DIRECTORY;


Game::Game(void)
{

}


void Game::Init(void)
{
	// Set up base variables and members
	mResourceManager = new ResourceManager();
	mCamera = new Camera("CAMERA");
	// Set up the base nodes
	mSceneGraph = new SceneGraph(mResourceManager);
	mSceneGraph->getRootNode()->addChildNode(mCamera);
	mMapGenerator = new MapGenerator(mSceneGraph, mResourceManager);

    // Run all initialization steps
    InitWindow();
    InitView();
    InitEventHandlers();

	srand(time(0));
	rand();
}


void Game::InitWindow(void){

    // Initialize the window management library (GLFW)
    if (!glfwInit()){
        throw(GameException(std::string("Could not initialize the GLFW library")));
    }

    // Create a window and its OpenGL context
    if (window_full_screen_g){
        mWindow = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), glfwGetPrimaryMonitor(), NULL);
    } else {
        mWindow = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), NULL, NULL);
    }
    if (!mWindow){
        glfwTerminate();
        throw(GameException(std::string("Could not create window")));
    }

    // Make the window's context the current one
    glfwMakeContextCurrent(mWindow);

    // Initialize the GLEW library to access OpenGL extensions
    // Need to do it after initializing an OpenGL context
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK){
        throw(GameException(std::string("Could not initialize the GLEW library: ")+std::string((const char *) glewGetErrorString(err))));
    }
}


void Game::InitView(void){

    // Set up z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);




    // Set viewport
    int width, height;
    glfwGetFramebufferSize(mWindow, &width, &height);
    glViewport(0, 0, width, height);

    // Set up camera
    // Set current view
    mCamera->SetView(camera_position_g, camera_look_at_g, camera_up_g);
    // Set projection
    mCamera->SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);
}


void Game::InitEventHandlers(void){

    // Set event callbacks
    glfwSetKeyCallback(mWindow, KeyCallback);
    glfwSetFramebufferSizeCallback(mWindow, ResizeCallback);

    // Set pointer to game object, so that callbacks can access it
    glfwSetWindowUserPointer(mWindow, (void *) this);
}


void Game::SetupResources(void){

	// Create a plane
	mResourceManager->CreateGrid("GridMesh");
	// Create a cube for the skybox
	mResourceManager->CreateCube("cubeMesh");
	mResourceManager->CreateCylinder("hayMesh");
	mResourceManager->CreateCylinder("PlayerMesh");

	std::string filename;
	std::string materials[] = { "default", "textured", "litTexture", "skybox" };
	for (std::string name : materials) {
		filename = std::string(shader_directory) + std::string("/" + name);
		mResourceManager->LoadResource(Material, name + "Material", filename.c_str());
	}

	filename = std::string(shader_directory) + std::string("/three-term_shiny_blue");
	mResourceManager->LoadResource(Material, "testMaterial", filename.c_str());


	std::string meshes[] = { "barn", "tree", "cow", "cannon", "farmer", "ufo" };
	for (std::string name : meshes) {
		filename = std::string(asset_directory) + std::string("/" + name + ".obj");
		mResourceManager->LoadResource(Mesh, name + "Mesh", filename.c_str());
	}

	std::string textures[] = { "ground", "hay", "tree", "barn", "cow", "bull", "cannon", "placeholder", "farmer", "ufo" };
	for (std::string name : textures) {
		// Load texture to be applied to the cube
		filename = std::string(asset_directory) + std::string("/" + name + ".png");
		mResourceManager->LoadResource(Texture, name + "Texture", filename.c_str());
	}

	//std::string skyboxes[] = { "Day1", "Dusk", "Night1", "Night2" };
	//for (std::string name : skyboxes) {
	//	// Load texture to be applied to the cube
	//	filename = std::string(asset_directory) + std::string("/skyboxes/" + name + "/" +name +".png");
	//	mResourceManager->LoadResource(CubeMap, name + "CubeMap", filename.c_str());
	//}
	
	filename = std::string(asset_directory) + std::string("/skyboxes/day1/day1.tga");
	mResourceManager->LoadResource(CubeMap, "Day1CubeMap", filename.c_str());

}


void Game::SetupScene(void){

    // Set background color for the scene
    mSceneGraph->SetBackgroundColor(viewport_background_color_g);

	// Create test Cow
	//CowEntityNode* cow1 = CreateInstance<CowEntityNode>("Cow1", "cowMesh", "texturedMaterial", "cowTexture");
	//cow1->Translate(glm::vec3(50.0, 0.0, 50.0));

	for (int i = 0; i < 40; i++)
	{
		CowEntityNode* cow = CreateInstance<CowEntityNode>("Cow" + std::to_string(i), "cowMesh", "texturedMaterial", "cowTexture");
		cow->Translate(glm::vec3((rand() % 300), 0.0, (rand() % 300)));
	}

	// Create test bull
	//BullEntityNode* bull1 = CreateInstance<BullEntityNode>("Bull1", "cowMesh", "texturedMaterial", "bullTexture");
	//bull1->Translate(glm::vec3(55.0, 0.0, 55.0));

	for (int i = 0; i < 20; i++)
	{
		BullEntityNode* bull = CreateInstance<BullEntityNode>("Bull" + std::to_string(i), "cowMesh", "texturedMaterial", "bullTexture");
		bull->Translate(glm::vec3((rand() % 300), 0.0, (rand() % 300)));
	}

	// Create test Farmer
	//FarmerEntityNode* farmer1 = CreateInstance<FarmerEntityNode>("Farmer1", "farmerMesh", "texturedMaterial", "farmerTexture");
	//farmer1->Scale(glm::vec3(0.75, 1.5, 0.75));
	//farmer1->Translate(glm::vec3(60.0, 0.0, 60.0));

	for (int i = 0; i < 20; i++)
	{
		FarmerEntityNode* farmer = CreateInstance<FarmerEntityNode>("Farmer" + std::to_string(i), "farmerMesh", "texturedMaterial", "farmerTexture");
		farmer->Scale(glm::vec3(0.75, 1.5, 0.75));
		farmer->Translate(glm::vec3((rand() % 300), 0.0, (rand() % 300)));
	}

	// Create test cannon
	//CannonMissileEntityNode* cannon1 = CreateInstance<CannonMissileEntityNode>("Cannon1", "cannonMesh", "litTextureMaterial", "cannonTexture");
	//cannon1->Scale(glm::vec3(2.0, 2.0, 2.0));
	//cannon1->Translate(glm::vec3(40.0, 0.0, 40.0));

	for (int i = 0; i < 5; i++)
	{
		CannonMissileEntityNode* cannon = CreateInstance<CannonMissileEntityNode>("Cannon" + std::to_string(i), "cannonMesh", "litTextureMaterial", "cannonTexture");
		cannon->Scale(glm::vec3(2.0, 2.0, 2.0));
		cannon->Translate(glm::vec3((rand() % 300), 0.0, (rand() % 300)));
	}

	SceneNode* player = CreatePlayerInstance("PLAYER", "ufoMesh", "litTextureMaterial", "ufoTexture");
	((PlayerNode*)player)->setPlayerPosition();
	mMapGenerator->GenerateMap();

	// Create skybox
	skybox_ = CreateInstance<SceneNode>("CubeInstance1", "cubeMesh", "skyboxMaterial", "Day1CubeMap");
	skybox_->Scale(glm::vec3(1000.0, 1000.0, 1000.0));

}


void Game::MainLoop(void){

    // Loop while the user did not close the window
    while (!glfwWindowShouldClose(mWindow)){
        // Animate the scene
        static double last_time = 0;
        double current_time = glfwGetTime();
        if ((current_time - last_time) > 0.05){
            mSceneGraph->Update();
            last_time = current_time;
			skybox_->SetPosition(mCamera->GetPosition());
        }

        // Draw the scene
        mSceneGraph->Draw(mCamera);

        // Push buffer drawn in the background onto the display
        glfwSwapBuffers(mWindow);

        // Update other events like input handling
        glfwPollEvents();
    }
}


void Game::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){

    // Get user data with a pointer to the game class
    void* ptr = glfwGetWindowUserPointer(window);
    Game *game = (Game *) ptr;

	PlayerNode *playerNode = (PlayerNode*)game->mSceneGraph->GetNode("PlayerTemp");

    // View control
    float rotFactor(glm::pi<float>() * 50.0f / 180);
    float transFactor = 3.0;
	float velocityFactor = 0.2f;
    if (key == GLFW_KEY_UP){
        game->mCamera->Pitch(rotFactor);
    }
    if (key == GLFW_KEY_DOWN){
        game->mCamera->Pitch(-rotFactor);
    }
    if (key == GLFW_KEY_LEFT){
        game->mCamera->Roll(-rotFactor);
    }
    if (key == GLFW_KEY_RIGHT){
        game->mCamera->Roll(rotFactor);
    }
    if (key == GLFW_KEY_Q){
		game->mCamera->Yaw(rotFactor);
    }
    if (key == GLFW_KEY_E){
		game->mCamera->Yaw(-rotFactor);
    }
    if (key == GLFW_KEY_W){
		game->mCamera->setVelocityForward(game->mCamera->getVelocityForward() + velocityFactor);
    }
    if (key == GLFW_KEY_S){
		game->mCamera->setVelocityForward(game->mCamera->getVelocityForward() - velocityFactor);
    }
	if (key == GLFW_KEY_A) {
		game->mCamera->setVelocitySide(game->mCamera->getVelocitySide() - velocityFactor / 2);
	}
	if (key == GLFW_KEY_D) {
		game->mCamera->setVelocitySide(game->mCamera->getVelocitySide() + velocityFactor / 2);
	}
	if (key == GLFW_KEY_LEFT_SHIFT) {
		game->mCamera->setVelocityUp(game->mCamera->getVelocityUp() + velocityFactor / 5);
	}
	if (key == GLFW_KEY_LEFT_CONTROL) {
		game->mCamera->setVelocityUp(game->mCamera->getVelocityUp() - velocityFactor / 5);
	}
    if (key == GLFW_KEY_J){
        game->mCamera->Translate(-game->mCamera->GetSide()*transFactor);
    }
    if (key == GLFW_KEY_L){
        game->mCamera->Translate(game->mCamera->GetSide()*transFactor);
    }
    if (key == GLFW_KEY_I){
        game->mCamera->Translate(game->mCamera->GetUp()*transFactor);
    }
    if (key == GLFW_KEY_K){
        game->mCamera->Translate(-game->mCamera->GetUp()*transFactor);
    }
	if (key == GLFW_KEY_X) {
		playerNode->rotateLeft();
	}
	if (key == GLFW_KEY_C) {
		playerNode->rotateRight();
	}
	if (key == GLFW_KEY_Y) {
		playerNode->rotateForward();
	}
	if (key == GLFW_KEY_U) {
		playerNode->rotateBackward();
	}
	if (key == GLFW_KEY_F) {
		game->mCamera->setVelocityForward(0.0f);
		game->mCamera->setVelocitySide(0.0f);
		game->mCamera->setVelocityUp(0.0f);
	}

}


void Game::ResizeCallback(GLFWwindow* window, int width, int height){

    // Set up viewport and camera projection based on new window size
    glViewport(0, 0, width, height);
    void* ptr = glfwGetWindowUserPointer(window);
    Game *game = (Game *) ptr;
    game->mCamera->SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);
}


Game::~Game(){

    glfwTerminate();
}

SceneNode *Game::CreatePlayerInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name) {

	Resource *geom = mResourceManager->GetResource(object_name);
	if (!geom) {
		throw(GameException(std::string("Could not find resource \"") + object_name + std::string("\"")));
	}

	Resource *mat = mResourceManager->GetResource(material_name);
	if (!mat) {
		throw(GameException(std::string("Could not find resource \"") + material_name + std::string("\"")));
	}

	Resource *tex = NULL;
	if (texture_name != "") {
		tex = mResourceManager->GetResource(texture_name);
		if (!tex) {
			throw(GameException(std::string("Could not find resource \"") + material_name + std::string("\"")));
		}
	}

	SceneNode* scn = mSceneGraph->CreatePlayerNode(entity_name, geom, mat, tex, mCamera);
	return scn;
}


} // namespace game
