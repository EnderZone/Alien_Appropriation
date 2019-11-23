#include <stdexcept>
#include <iostream>
#include <fstream>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "scene_graph.h"

namespace game {

SceneGraph::SceneGraph(void){

    background_color_ = glm::vec3(0.0, 0.0, 0.0);
}


SceneGraph::~SceneGraph(){
}


void SceneGraph::SetBackgroundColor(glm::vec3 color){

    background_color_ = color;
}


glm::vec3 SceneGraph::GetBackgroundColor(void) const {

    return background_color_;
}
 

SceneNode *SceneGraph::CreateNode(std::string node_name, Resource *geometry, Resource *material, Resource *texture){

    // Create scene node with the specified resources
    SceneNode *scn = new SceneNode(node_name, geometry, material, texture);

    // Add node to the scene
    node_.push_back(scn);

    return scn;
}


void SceneGraph::AddNode(SceneNode *node){

    node_.push_back(node);
}


SceneNode *SceneGraph::GetNode(std::string node_name) const {

    // Find node with the specified name
    for (int i = 0; i < node_.size(); i++){
        if (node_[i]->GetName() == node_name){
            return node_[i];
        }
    }
    return NULL;

}


std::vector<SceneNode *>::const_iterator SceneGraph::begin() const {

    return node_.begin();
}


std::vector<SceneNode *>::const_iterator SceneGraph::end() const {

    return node_.end();
}


void SceneGraph::Draw(Camera *camera){

    // Clear background
    glClearColor(background_color_[0], 
                 background_color_[1],
                 background_color_[2], 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw all scene nodes
    for (int i = 0; i < node_.size(); i++){
        node_[i]->Draw(camera);
    }
}


void SceneGraph::Update(void){

    for (int i = 0; i < node_.size(); i++){
        node_[i]->Update();
    }
}


void SceneGraph::SetupDrawToTexture(void){

    // Set up frame buffer
    glGenFramebuffers(1, &frame_buffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_);

    // Set up target texture for rendering
    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);

    // Set up an image for the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Set up a depth buffer for rendering
    glGenRenderbuffers(1, &depth_buffer_);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

    // Configure frame buffer (attach rendering buffers)
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer_);
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);

    // Check if frame buffer was setup successfully 
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
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
    glGenBuffers(1, &quad_array_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, quad_array_buffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertex_data), quad_vertex_data, GL_STATIC_DRAW);
}


void SceneGraph::DrawToTexture(Camera *camera){

    // Save current viewport
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    // Enable frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_);
    glViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT); 

    // Clear background
    glClearColor(background_color_[0], 
                 background_color_[1],
                 background_color_[2], 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw all scene nodes
    for (int i = 0; i < node_.size(); i++){
        node_[i]->Draw(camera);
    }

    // Reset frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Restore viewport
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}


void SceneGraph::DisplayTexture(GLuint program){

    // Configure output to the screen
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);

    // Set up quad geometry
    glBindBuffer(GL_ARRAY_BUFFER, quad_array_buffer_);

    // Select proper material (shader program)
    glUseProgram(program);

    // Setup attributes of screen-space shader
    GLint pos_att = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(pos_att);
    glVertexAttribPointer(pos_att, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

    GLint tex_att = glGetAttribLocation(program, "uv");
    glEnableVertexAttribArray(tex_att);
    glVertexAttribPointer(tex_att, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *) (3*sizeof(GLfloat)));

    // Timer
    GLint timer_var = glGetUniformLocation(program, "timer");
    float current_time = glfwGetTime();
    glUniform1f(timer_var, current_time);

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_);

    // Draw geometry
    glDrawArrays(GL_TRIANGLES, 0, 6); // Quad: 6 coordinates

    // Reset current geometry
    glEnable(GL_DEPTH_TEST);
}


void SceneGraph::SaveTexture(char *filename){

    unsigned char data[FRAME_BUFFER_WIDTH*FRAME_BUFFER_HEIGHT*4];

    // Retrieve image data from texture
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_);
    glReadPixels(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // Create file in ppm format
    // Open the file
    std::ofstream f;
    f.open(filename);
    if (f.fail()){
        throw(std::ios_base::failure(std::string("Error opening file ")+std::string(filename)));
    }

    // Write header
    f << "P3" << std::endl;
    f << FRAME_BUFFER_WIDTH << " " << FRAME_BUFFER_HEIGHT << std::endl;
    f << "255" << std::endl;

    // Write data
    for (int i = 0; i < FRAME_BUFFER_HEIGHT; i++){
        for (int j = 0; j < FRAME_BUFFER_WIDTH; j++){
            for (int k = 0; k < 3; k++){
                int dt = data[i*FRAME_BUFFER_WIDTH*4 + j*4 + k];
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

} // namespace game
