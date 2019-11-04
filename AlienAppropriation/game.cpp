#include <iostream>
#include <time.h>
#include <sstream>

#include "game.h"
#include "bin/path_config.h"

namespace game {

// Some configuration constants
// They are written here as global variables, but ideally they should be loaded from a configuration file

// Main window settings
const std::string window_title_g = "Demo";
const unsigned int window_width_g = 800;
const unsigned int window_height_g = 600;
const bool window_full_screen_g = false;

// Viewport and camera settings
float camera_near_clip_distance_g = 0.01;
float camera_far_clip_distance_g = 1000.0;
float camera_fov_g = 20.0; // Field-of-view of camera
const glm::vec3 viewport_background_color_g(0.0, 0.0, 0.0);
glm::vec3 camera_position_g(0.0, 0.0, 800.0);
glm::vec3 camera_look_at_g(0.0, 0.0, 0.0);
glm::vec3 camera_up_g(0.0, 1.0, 0.0);

// Materials 
const std::string material_directory_g = MATERIAL_DIRECTORY;


Game::Game(void)
{

}


void Game::Init(void)
{
	// Set up the base nodes
	mSceneGraph = new SceneGraph();
	mCamera = new Camera("CAMERA");
	mSceneGraph->getRootNode()->addChildNode(mCamera);

    // Run all initialization steps
    InitWindow();
    InitView();
    InitEventHandlers();

    // Set variables
    mAnimating = true;
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

    // Create a simple sphere to represent the asteroids
    mResourceManager.CreateSphere("SimpleSphereMesh", 1.0, 10, 10);

	// Create simple cylinder for the ray
	mResourceManager.CreateCylinder("SimpleCylinderMesh", 100000.0f, 0.05f, 30);

	// Create cylinders for the cannon
	mResourceManager.CreateCylinder("CannonMesh1", 0.4f, 0.15f);
	mResourceManager.CreateCylinder("CannonMesh2", 0.3f, 0.4f);
	mResourceManager.CreateCylinder("CannonMesh3", 0.4f, 0.065f);
	mResourceManager.CreateCylinder("CannonMesh4", 0.5f, 0.035f);

	// Create simple torus for the player ship
	mResourceManager.CreateTorus("SimpleTorusMesh");

	// Create simple square for ground plane
	mResourceManager.CreateSquare("SimpleSquareMesh", 50.0f);

    // Load material to be applied to asteroids
    std::string filename = std::string(MATERIAL_DIRECTORY) + std::string("/material");
    mResourceManager.LoadResource(Material, "ObjectMaterial", filename.c_str());
}


void Game::SetupScene(void){

    // Set background color for the scene
    mSceneGraph->SetBackgroundColor(viewport_background_color_g);

	// Create the ground plane
	CreatePlane();

    // Create asteroid field
    CreateAsteroidField();

	// Create Cannon
	CreateCannon();

	// Create Player Ship
	CreatePlayerShip();
}


void Game::MainLoop(void){

    // Loop while the user did not close the window
    while (!glfwWindowShouldClose(mWindow)){
        // Animate the scene
        if (mAnimating){
            static double last_time = 0;
            double current_time = glfwGetTime();
            if ((current_time - last_time) > 0.05){
                mSceneGraph->Update();
                last_time = current_time;
            }
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
    if (key == GLFW_KEY_A || key == GLFW_KEY_LEFT){
        game->mCamera->Yaw(rotFactor);
    }
    if (key == GLFW_KEY_D || key == GLFW_KEY_RIGHT){
        game->mCamera->Yaw(-rotFactor);
    }
    if (key == GLFW_KEY_Q){
        game->mCamera->Roll(-rotFactor);
    }
    if (key == GLFW_KEY_E){
        game->mCamera->Roll(rotFactor);
    }
    if (key == GLFW_KEY_W){
		if (game->mCamera->getVelocity() <= transFactor)
			game->mCamera->setVelocity(game->mCamera->getVelocity() + velocityFactor);
    }
    if (key == GLFW_KEY_S){
		if (game->mCamera->getVelocity() >= -transFactor)
			game->mCamera->setVelocity(game->mCamera->getVelocity() - velocityFactor);
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

	if (key == GLFW_KEY_F) {
		game->mCamera->setVelocity(0.0f);
	}

	// Swap Camera Perspective if 'r' is pressed
	if (key == GLFW_KEY_R && action == GLFW_PRESS) 
	{
		game->mCamera->SwitchCameraPerspective();
	}

	// Fire laser if space bar is pressed
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) 
	{
		//todo fire laser
		game->CreateLaser();
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


Asteroid *Game::CreateAsteroidInstance(std::string entity_name, std::string object_name, std::string material_name)
{

    // Get resources
    Resource *geom = mResourceManager.GetResource(object_name);
    if (!geom){
        throw(GameException(std::string("Could not find resource \"")+object_name+std::string("\"")));
    }

    Resource *mat = mResourceManager.GetResource(material_name);
    if (!mat){
        throw(GameException(std::string("Could not find resource \"")+material_name+std::string("\"")));
    }

    // Create asteroid instance
    Asteroid *ast = new Asteroid(entity_name, geom, mat);
    mSceneGraph->AddNode(ast);
    return ast;
}


void Game::CreateAsteroidField(int num_asteroids){

    // Create a number of asteroid instances
    for (int i = 0; i < num_asteroids; i++){
        // Create instance name
        std::stringstream ss;
        ss << i;
        std::string index = ss.str();
        std::string name = "AsteroidInstance" + index;

        // Create asteroid instance
        Asteroid *ast = CreateAsteroidInstance(name, "SimpleSphereMesh", "ObjectMaterial");

        // Set attributes of asteroid: random position, orientation, and
        // angular momentum
        ast->SetPosition(glm::vec3(-300.0 + 600.0*((float) rand() / RAND_MAX), -300.0 + 600.0*((float) rand() / RAND_MAX), 600.0*((float) rand() / RAND_MAX)));
		//ast->SetPosition(glm::vec3(0, 0, 775));
        ast->SetOrientation(glm::normalize(glm::angleAxis(glm::pi<float>()*((float) rand() / RAND_MAX), glm::vec3(((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX)))));
        ast->SetAngM(glm::normalize(glm::angleAxis(0.05f*glm::pi<float>()*((float) rand() / RAND_MAX), glm::vec3(((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX)))));

		ast->setRadius(1.0f);
    }
}

void Game::CreateCannon()
{
	// Get resources
	Resource *geom;

	//Camera* cameraNode = (Camera*)mSceneGraph->GetNode("CAMERA");

	Resource *mat = mResourceManager.GetResource("ObjectMaterial");
	if (!mat) {
		throw(GameException(std::string("Could not find resource \"") + "ObjectMaterial" + std::string("\"")));
	}

	// Cannon Piece 1
	geom = mResourceManager.GetResource("CannonMesh1");
	if (!geom) {
		throw(GameException(std::string("Could not find resource \"") + "CannonMesh1" + std::string("\"")));
	}

	Asteroid* cannon1 = new Asteroid("Cannon1", geom, mat);
	cannon1->SetPosition(glm::vec3(0, -19.5f, 750));
	cannon1->Translate(glm::vec3(0, 0, 0));
	cannon1->SetAngM(glm::normalize(glm::angleAxis(glm::radians(3.0f), glm::vec3(0.0, 1.0, 0.0))));
	mSceneGraph->getRootNode()->addChildNode(cannon1);

	// Cannon Piece 2
	geom = mResourceManager.GetResource("CannonMesh2");
	if (!geom) {
		throw(GameException(std::string("Could not find resource \"") + "CannonMesh2" + std::string("\"")));
	}

	Asteroid* cannon2 = new Asteroid("Cannon2", geom, mat);
	cannon2->Translate(glm::vec3(0, 0.3, 0));
	cannon1->addChildNode(cannon2);

	// Cannon Piece 3
	geom = mResourceManager.GetResource("CannonMesh3");
	if (!geom) {
		throw(GameException(std::string("Could not find resource \"") + "CannonMesh3" + std::string("\"")));
	}

	Asteroid* cannon3 = new Asteroid("Cannon3", geom, mat);
	cannon3->Translate(glm::vec3(-0.4, 0, 0));
	cannon3->SetOrientation(glm::normalize(glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0))));
	cannon2->addChildNode(cannon3);

	// Cannon Piece 4
	geom = mResourceManager.GetResource("CannonMesh4");
	if (!geom) {
		throw(GameException(std::string("Could not find resource \"") + "CannonMesh4" + std::string("\"")));
	}

	Asteroid* cannon4 = new Asteroid("Cannon4", geom, mat);
	cannon4->Translate(glm::vec3(0, 0.3, 0));
	cannon3->addChildNode(cannon4);
}


void Game::CreatePlayerShip()
{
	// Get resources
	Resource *geom = mResourceManager.GetResource("SimpleTorusMesh");
	if (!geom) {
		throw(GameException(std::string("Could not find resource \"") + "SimpleTorusMesh" + std::string("\"")));
	}

	Resource *mat = mResourceManager.GetResource("ObjectMaterial");
	if (!mat) {
		throw(GameException(std::string("Could not find resource \"") + "ObjectMaterial" + std::string("\"")));
	}

	Camera* cameraNode = (Camera*)mSceneGraph->GetNode("CAMERA");

	SceneNode* playerShip = new SceneNode("PlayerShip", geom, mat);
	playerShip->Translate(glm::vec3(0, -0.2f, 0));

	cameraNode->addChildNode(playerShip);
}


void Game::CreateLaser()
{
	if (mSceneGraph->GetNode("PlayerLaser") == nullptr)
	{
		// Get resources
		Resource *geom = mResourceManager.GetResource("SimpleCylinderMesh");
		if (!geom) {
			throw(GameException(std::string("Could not find resource \"") + "SimpleCylinderMesh" + std::string("\"")));
		}

		Resource *mat = mResourceManager.GetResource("ObjectMaterial");
		if (!mat) {
			throw(GameException(std::string("Could not find resource \"") + "ObjectMaterial" + std::string("\"")));
		}

		SceneNode* playerShipNode = (SceneNode*)mSceneGraph->GetNode("PlayerShip");

		ProjectileNode* laser = new ProjectileNode("PlayerLaser", geom, mat, 1.0f);
		laser->Translate(glm::vec3(0, 0, -50000.0f));
		laser->SetOrientation(glm::normalize(glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0))));
		playerShipNode->addChildNode(laser);
	}
}


void Game::CreatePlane()
{
	// Get resources
	Resource *geom = mResourceManager.GetResource("SimpleSquareMesh");
	if (!geom) {
		throw(GameException(std::string("Could not find resource \"") + "SimpleSquareMesh" + std::string("\"")));
	}

	Resource *mat = mResourceManager.GetResource("ObjectMaterial");
	if (!mat) {
		throw(GameException(std::string("Could not find resource \"") + "ObjectMaterial" + std::string("\"")));
	}

	SceneNode* planeNode = new SceneNode("Plane", geom, mat);
	planeNode->Translate(glm::vec3(0.0f, -20.0f, 750.0f));
	planeNode->SetOrientation(glm::normalize(glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0))));

	mSceneGraph->getRootNode()->addChildNode(planeNode);
}


} // namespace game
