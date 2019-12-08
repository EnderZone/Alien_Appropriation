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
	mCamera = new Camera("camera");
	// Set up the base nodes
	mSceneGraph = new SceneGraph(mCamera);
	mMapGenerator = new MapGenerator(mSceneGraph);

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
	mResourceManager->CreateCone("coneMesh");
	mResourceManager->CreateSphereParticles("shieldMesh");
	mResourceManager->CreateSquare("healthMesh", 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	mResourceManager->CreateSquare("energyMesh", 1.0f, glm::vec3(0.0f, 0.7f, 0.7f));

	std::string filename;
	std::string materials[] = { "default", "textured", "litTexture", "skybox", "particle" };
	for (std::string name : materials) {
		filename = std::string(shader_directory) + std::string("/" + name);
		mResourceManager->LoadResource(Material, name + "Material", filename.c_str());
	}

	filename = std::string(shader_directory) + std::string("/three-term_shiny_blue");
	mResourceManager->LoadResource(Material, "testMaterial", filename.c_str());


	std::string meshes[] = { "barn", "tree", "cow", "cannon", "farmer", "ufo", "missile"};
	for (std::string name : meshes) {
		filename = std::string(asset_directory) + std::string("/" + name + ".obj");
		mResourceManager->LoadResource(Mesh, name + "Mesh", filename.c_str());
	}

	std::string textures[] = { "placeholder", "ground", "hay", "tree", "barn", "cow", "bull", "cannon", "farmer", "ufo", "missile" };
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

	for (int i = 0; i < 40; i++)
	{
		CowEntityNode* cow = mSceneGraph->CreateInstance<CowEntityNode>("Cow" + std::to_string(i), "cowMesh", "texturedMaterial", "cowTexture");
		cow->translate(glm::vec3((rand() % 300), 0.0, (rand() % 300)));
	}

	for (int i = 0; i < 20; i++)
	{
		BullEntityNode* bull = mSceneGraph->CreateInstance<BullEntityNode>("Bull" + std::to_string(i), "cowMesh", "texturedMaterial", "bullTexture");
		bull->translate(glm::vec3((rand() % 300), 0.0, (rand() % 300)));
	}

	for (int i = 0; i < 20; i++)
	{
		FarmerEntityNode* farmer = mSceneGraph->CreateInstance<FarmerEntityNode>("Farmer" + std::to_string(i), "farmerMesh", "texturedMaterial", "farmerTexture");
		farmer->scale(glm::vec3(0.75, 1.5, 0.75));
		farmer->translate(glm::vec3((rand() % 300), 0.0, (rand() % 300)));
	}

	for (int i = 0; i < 5; i++)
	{
		CannonMissileEntityNode* cannon = mSceneGraph->CreateInstance<CannonMissileEntityNode>("Cannon" + std::to_string(i), "cannonMesh", "litTextureMaterial", "cannonTexture");
		cannon->scale(glm::vec3(2.0, 2.0, 2.0));
		cannon->translate(glm::vec3((rand() % 300), 0.0, (rand() % 300)));
	}

	// stats for the player and ui nodes to hold
	float* max_stat = new float(100);
	float* health = new float(100);
	float* shield = new float(100);

	PlayerNode* player = mSceneGraph->CreateInstance<PlayerNode>("player", "ufoMesh", "litTextureMaterial", "ufoTexture", mCamera);
	mSceneGraph->setPlayerNode(player);
	player->setPlayerPosition();

	//Create tractor beam
	SceneNode* weapon = mSceneGraph->CreateInstance<SceneNode>("TRACTORBEAM", "coneMesh", "defaultMaterial");
	mSceneGraph->getRootNode()->removeChildNode("TRACTORBEAM");
	player->addWeapon(weapon);
	weapon->translate(glm::vec3(0.0, 0.0, 0.0));
	weapon->scale(glm::vec3(10.0, 50.0, 10.0));
	((PlayerNode*)player)->addHealthTracker(health);
	((PlayerNode*)player)->addEnergyTracker(shield);


	//Create shields
	weapon = mSceneGraph->CreateInstance<SceneNode>("SHIELD", "shieldMesh", "particleMaterial");
	mSceneGraph->getRootNode()->removeChildNode("SHIELD");
	player->addWeapon(weapon);
	weapon->translate(glm::vec3(0.0, 0.0, 0.0));
	weapon->scale(glm::vec3(10.0, 10.0, 10.0));

	mMapGenerator->GenerateMap();


	//Create UI elements
	SceneNode* ui_nodes = mSceneGraph->CreateInstance<UINode>("HEALTH_UI", "healthMesh", "defaultMaterial");
	mSceneGraph->getRootNode()->removeChildNode("HEALTH_UI");
	player->addChildNode(ui_nodes);
	ui_nodes->translate(glm::vec3(0.5f, 1.0f, 2.0f));
	ui_nodes->rotate(glm::angleAxis(glm::pi<float>() / 5, glm::vec3(0.0f, 1.0f, 0.0f)));
	((UINode*)ui_nodes)->addStat(health, max_stat);

	ui_nodes = mSceneGraph->CreateInstance<UINode>("SHIELD_UI", "energyMesh", "defaultMaterial");
	mSceneGraph->getRootNode()->removeChildNode("SHIELD_UI");
	player->addChildNode(ui_nodes);
	ui_nodes->translate(glm::vec3(0.5f, 1.0f, -2.0f));
	ui_nodes->rotate(glm::angleAxis(glm::pi<float>() / 5, glm::vec3(0.0f, 1.0f, 0.0f)));
	((UINode*)ui_nodes)->addStat(shield, max_stat);

	// Create skybox
	skybox_ = mSceneGraph->CreateInstance<SceneNode>("CubeInstance1", "cubeMesh", "skyboxMaterial", "Day1CubeMap");
	skybox_->scale(glm::vec3(1000.0, 1000.0, 1000.0));

}


void Game::MainLoop(void){

    // Loop while the user did not close the window
    while (!glfwWindowShouldClose(mWindow)){
        // Animate the scene
        static double last_time = 0;
        double current_time = glfwGetTime();
		double deltaTime = current_time - last_time;
        if ((current_time - last_time) > 0.05){
            mSceneGraph->update(deltaTime);
            last_time = current_time;
			skybox_->setPosition(mCamera->getPosition());
        }

        // draw the scene
        mSceneGraph->draw(mCamera);

        // Push buffer drawn in the background onto the display
        glfwSwapBuffers(mWindow);

        // update other events like input handling
        glfwPollEvents();
    }
}


void Game::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){

    // Get user data with a pointer to the game class
    void* ptr = glfwGetWindowUserPointer(window);
    Game *game = (Game *) ptr;

	PlayerNode *playerNode = (PlayerNode*)game->mSceneGraph->getPlayerNode();

    // View control
    float rotFactor(glm::pi<float>() * 1  / 180);
    float transFactor = 3.0;
	float velocityFactor = 0.2f;
    if (key == GLFW_KEY_UP){
        game->mCamera->Pitch(rotFactor);
    }
    if (key == GLFW_KEY_DOWN){
        game->mCamera->Pitch(-rotFactor);
    }
    if (key == GLFW_KEY_Q){
		game->mCamera->Yaw(rotFactor);
    }
    if (key == GLFW_KEY_E){
		game->mCamera->Yaw(-rotFactor);
    }
    if (key == GLFW_KEY_W){
		//game->mCamera->setVelocityForward(game->mCamera->getVelocityForward() + velocityFactor);
		game->mCamera->addVelocity(glm::vec3(0, 0, velocityFactor));
    }
    if (key == GLFW_KEY_S){
		//game->mCamera->setVelocityForward(game->mCamera->getVelocityForward() - velocityFactor);
		game->mCamera->addVelocity(glm::vec3(0, 0, -velocityFactor));

    }
	if (key == GLFW_KEY_A) {
		//game->mCamera->setVelocitySide(game->mCamera->getVelocitySide() - velocityFactor / 2);
		game->mCamera->addVelocity(glm::vec3(-velocityFactor/2, 0, 0));

	}
	if (key == GLFW_KEY_D) {
		//game->mCamera->setVelocitySide(game->mCamera->getVelocitySide() + velocityFactor / 2);
		game->mCamera->addVelocity(glm::vec3(velocityFactor / 2, 0, 0));

	}
	if (key == GLFW_KEY_LEFT_SHIFT) {
		//game->mCamera->setVelocityUp(game->mCamera->getVelocityUp() + velocityFactor / 5);
		game->mCamera->addVelocity(glm::vec3(0, velocityFactor / 5, 0));

	}
	if (key == GLFW_KEY_LEFT_CONTROL) {
		//game->mCamera->setVelocityUp(game->mCamera->getVelocityUp() - velocityFactor / 5);
		game->mCamera->addVelocity(glm::vec3(0, -velocityFactor / 5, 0));

	}
	if (key == GLFW_KEY_SPACE) {
		if (action == GLFW_PRESS) playerNode->toggleTractorBeam(true);
		if (action == GLFW_RELEASE) playerNode->toggleTractorBeam(false);

	}
	if (key == GLFW_KEY_C) {
		if (action == GLFW_PRESS) playerNode->toggleShields(true);
		if (action == GLFW_RELEASE) playerNode->toggleShields(false);
	}
	if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
		game->mCamera->SwitchCameraPerspective();
	}
	if (key == GLFW_KEY_Y) {
		playerNode->rotateForward();
	}
	if (key == GLFW_KEY_U) {
		playerNode->rotateBackward();
	}
	if (key == GLFW_KEY_F) {
		game->mCamera->setVelocity(glm::vec3(0));
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

} // namespace game
