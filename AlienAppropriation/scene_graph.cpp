#include <stdexcept>
#include <iostream>
#include <fstream>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "scene_graph.h"

#include "scene_node.h"

namespace game {

SceneGraph::SceneGraph(void){

    mBackgroundColor = glm::vec3(0.0, 0.0, 0.0);

	mRootNode = new BaseNode("ROOT");
	
}


SceneGraph::~SceneGraph(){
}


void SceneGraph::SetBackgroundColor(glm::vec3 color){

    mBackgroundColor = color;
}


glm::vec3 SceneGraph::GetBackgroundColor(void) const {

    return mBackgroundColor;
}



SceneNode *SceneGraph::CreatePlayerNode(std::string node_name, Resource *geometry, Resource *material, Resource *texture, BaseNode* camera)
{
	// Create scene node with the specified resources
	PlayerNode* pn = new PlayerNode(node_name, geometry, material, camera);

	// Add node to the scene
	mRootNode->addChildNode((SceneNode*)pn);

	return (SceneNode*)pn;
}


void SceneGraph::AddNode(SceneNode *node)
{
    //node_.push_back(node);
	mRootNode->addChildNode(node);
}


game::BaseNode* SceneGraph::GetNode(std::string node_name, BaseNode* currentNode /*= nullptr*/) const 
{
	if (currentNode == nullptr)
		currentNode = mRootNode;

	if (currentNode->getName() == node_name)
		return currentNode;
	else
	{
		for (BaseNode* node : currentNode->getChildNodes())
		{
			if (GetNode(node_name, node) != nullptr)
				return GetNode(node_name, node);
		}
		return nullptr;
	}

    return nullptr;
}



void SceneGraph::Draw(Camera *camera)
{
    // Clear background
    glClearColor(mBackgroundColor[0], 
                 mBackgroundColor[1],
                 mBackgroundColor[2], 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mRootNode->Draw(camera);
}

void SceneGraph::SetupDrawToTexture(void) {

	// Set up frame buffer
	glGenFramebuffers(1, &mFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);

	// Set up target texture for rendering
	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);

	// Set up an image for the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Set up a depth buffer for rendering
	glGenRenderbuffers(1, &mDepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, mDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

	// Configure frame buffer (attach rendering buffers)
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthBuffer);
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);

	// Check if frame buffer was setup successfully 
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw(std::ios_base::failure(std::string("Error setting up frame buffer")));
	}

	// Reset frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Set up quad for drawing to the screen
	static const GLfloat quad_vertex_data[] = {
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
	};

	// Create buffer for quad
	glGenBuffers(1, &mQuadArrayBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mQuadArrayBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertex_data), quad_vertex_data, GL_STATIC_DRAW);
}


void SceneGraph::DrawToTexture(Camera *camera) {

	// Save current viewport
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Enable frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
	glViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

	// Clear background
	glClearColor(mBackgroundColor[0],
		mBackgroundColor[1],
		mBackgroundColor[2], 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw all scene nodes
	mRootNode->Draw(camera);

	// Reset frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Restore viewport
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}


void SceneGraph::DisplayTexture(GLuint program) {

	// Configure output to the screen
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);

	// Set up quad geometry
	glBindBuffer(GL_ARRAY_BUFFER, mQuadArrayBuffer);

	// Select proper material (shader program)
	glUseProgram(program);

	// Setup attributes of screen-space shader
	GLint pos_att = glGetAttribLocation(program, "position");
	glEnableVertexAttribArray(pos_att);
	glVertexAttribPointer(pos_att, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

	GLint tex_att = glGetAttribLocation(program, "uv");
	glEnableVertexAttribArray(tex_att);
	glVertexAttribPointer(tex_att, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));

	// Timer
	GLint timer_var = glGetUniformLocation(program, "timer");
	float current_time = glfwGetTime();
	glUniform1f(timer_var, current_time);

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTexture);

	// Draw geometry
	glDrawArrays(GL_TRIANGLES, 0, 6); // Quad: 6 coordinates

									  // Reset current geometry
	glEnable(GL_DEPTH_TEST);
}


void SceneGraph::SaveTexture(char *filename) {

	unsigned char data[FRAME_BUFFER_WIDTH*FRAME_BUFFER_HEIGHT * 4];

	// Retrieve image data from texture
	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
	glReadPixels(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, data);

	// Create file in ppm format
	// Open the file
	std::ofstream f;
	f.open(filename);
	if (f.fail()) {
		throw(std::ios_base::failure(std::string("Error opening file ") + std::string(filename)));
	}

	// Write header
	f << "P3" << std::endl;
	f << FRAME_BUFFER_WIDTH << " " << FRAME_BUFFER_HEIGHT << std::endl;
	f << "255" << std::endl;

	// Write data
	for (int i = 0; i < FRAME_BUFFER_HEIGHT; i++) {
		for (int j = 0; j < FRAME_BUFFER_WIDTH; j++) {
			for (int k = 0; k < 3; k++) {
				int dt = data[i*FRAME_BUFFER_WIDTH * 4 + j * 4 + k];
				f << dt << " ";
			}
		}
		f << std::endl;
	}

	// Close the file
	f.close();

	// Reset frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}


void SceneGraph::Update(void)
{
	mRootNode->Update();
}

} // namespace game
