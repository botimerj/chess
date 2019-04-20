#ifndef UI_H
#define UI_H

#include <vector>
#include <iostream>
#include <thread>

// Display libraries
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <shader.h>
#include <texture.h>

// GLM libraries
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Custom libraries
#include <resource_manager.h>
#include <game.h>
#include <board.h>
#include <tbox.h>

// This class defines the user interface.
class UI{
public:

    // Display
    GLFWwindow *window;
    int WIDTH;
    int HEIGHT;
    float aspect_ratio;
    glm::mat4 projection;

    // Shaders and textures and fonts
    Resource_manager * rm;

    // Mouse
    glm::vec2 mouse_pos;

    // Partitions
    Board * board;
    Tbox  * tbox;

    // Game State
    Game * game;

    // UI Functions
    UI(GLFWwindow *window, int WIDTH, int HEIGHT); 
    ~UI();
    void resize_window(int width, int height);
    void render();

    void left_click(int action);
    void right_click(int action);
    void get_mouse_pos();

    // stdin handler
    bool exit;
    std::thread t_listen;
    void listen();

};

#endif
