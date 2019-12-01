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
const glm::vec3 viewport_background_color_g(0.0, 0.0, 0.0);
glm::vec3 camera_position_g(0.0, 40.0, 120.0);
glm::vec3 camera_look_at_g(0.0, 0.0, 0.0);
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
	mSceneGraph = new SceneGraph();
	mSceneGraph->getRootNode()->addChildNode(mCamera);

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

	// Load generic materials
	// Create a square
	mResourceManager->CreateCylinder("PlayerMesh");

	// Load a generic material
	std::string filename = std::string(shader_directory) + std::string("/material");
	mResourceManager->LoadResource(Material, "BasicMaterial", filename.c_str());

	filename = std::string(shader_directory) + std::string("/textured_material");
	mResourceManager->LoadResource(Material, "TexturedMaterial", filename.c_str());

	// Load a test cube from an obj file
	filename = std::string(asset_directory) + std::string("/cube.obj");
	mResourceManager->LoadResource(Mesh, "CubeMesh", filename.c_str());

	// Load texture to be applied to the cube
	filename = std::string(asset_directory) + std::string("/texture.png");
	mResourceManager->LoadResource(Texture, "Texture", filename.c_str());
}


void Game::SetupScene(void){

    // Set background color for the scene
    mSceneGraph->SetBackgroundColor(viewport_background_color_g);

	// Create ground for everything
	SceneNode* ground = CreateInstance<SceneNode>("Ground", "GridMesh", "BasicMaterial");
	ground->Translate(glm::vec3(-50.0, 0.0, -50.0));

	// Create test Cow
	CowEntityNode* cow1 = CreateInstance<CowEntityNode>("Cow1", "CubeMesh", "TexturedMaterial", "Texture");
	cow1->Translate(glm::vec3(-3.0, 1.0, 0.0));

	// Create test bull
	BullEntityNode* bull1 = CreateInstance<BullEntityNode>("Bull1", "CubeMesh", "TexturedMaterial", "Texture");
	bull1->Translate(glm::vec3(3.0, 1.0, 0.0));

	// Create test Farmer
	FarmerEntityNode* farmer1 = CreateInstance<FarmerEntityNode>("Farmer1", "CubeMesh", "TexturedMaterial", "Texture");
	farmer1->Scale(glm::vec3(0.75, 1.5, 0.75));
	farmer1->Translate(glm::vec3(0.0, 1.5, 0.0));

	// Create test cannon
	CannonMissileEntityNode* cannon1 = CreateInstance<CannonMissileEntityNode>("Cannon1", "CubeMesh", "TexturedMaterial", "Texture");
	cannon1->Scale(glm::vec3(2.0, 2.0, 2.0));
	cannon1->Translate(glm::vec3(0.0, 2.0, -20.0));

	SceneNode* player = CreatePlayerInstance("PLAYER", "PlayerMesh", "BasicMaterial");
	player->Translate(glm::vec3(0,0,-20));
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
    float rotFactor(glm::pi<float>() / 180);
    float transFactor = 3.0;
	float velocityFactor = 0.2f;
    if (key == GLFW_KEY_UP){
        game->mCamera->Pitch(rotFactor);
    }
    if (key == GLFW_KEY_DOWN){
        game->mCamera->Pitch(-rotFactor);
    }
    if (key == GLFW_KEY_LEFT){
        game->mCamera->Yaw(rotFactor);
    }
    if (key == GLFW_KEY_RIGHT){
        game->mCamera->Yaw(-rotFactor);
    }
    if (key == GLFW_KEY_Q){
        game->mCamera->Roll(-rotFactor);
    }
    if (key == GLFW_KEY_E){
        game->mCamera->Roll(rotFactor);
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
