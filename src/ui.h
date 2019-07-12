#ifndef UI_H
#define UI_H

#include <vector>
#include <iostream>
#include <thread>
#include <cctype>
#include <unistd.h>     // getpid()
#include <sys/types.h>

#define PIPE_READ 0
#define PIPE_WRITE 1

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
#include <menu.h>




// This class defines the user interface.
class UI{
public:

    // Display
    GLFWwindow *window;
    int WIDTH;
    int HEIGHT;
    float aspect_ratio;
    glm::mat4 projection;
    bool render_continuously;
    bool render_flag;

    // Shaders and textures and fonts
    Resource_manager * rm;

    // Mouse
    glm::vec2 mouse_pos;

    // Partitions
    Board * board;
    Tbox  * tbox;
    Menu  * menu;

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
    bool valid_move_string(char * in);

    // agent handler
    bool kill_agent_w;
    bool kill_agent_b;
    std::thread t_agent_w;
    std::thread t_agent_b;
    void agent_handler(int color, const char * agent);
    void write_log_file(int pid, const char * status);
    void clear_log_file();
};

#endif
