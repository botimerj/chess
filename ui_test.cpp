
#include <stdio.h> 
#include <stdlib.h>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include <shader.h>
#include <texture.h>
#include <ui.h>
#include <math.h>

#define  STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Chess", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // Call backs
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    //glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Enable blending
    glViewport(0,0,SCR_WIDTH,SCR_HEIGHT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //float ar = static_cast<float>(SCR_WIDTH)/static_cast<float>(SCR_HEIGHT);
    UI ui(window, SCR_WIDTH, SCR_HEIGHT);
    glfwSetWindowUserPointer(window, &ui);

    double xpos, ypos;
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

    // render loop
    while (!glfwWindowShouldClose(window)){

        // input
        //gs.processInput();

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ui.render();

        // glfw: swap buffers and poll IO events 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate
    glfwTerminate();
    return 0;
}

// glfw: whenever the window size changed this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
    UI *uiPtr = static_cast<UI*>(glfwGetWindowUserPointer(window));
    uiPtr->resize_window(width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    UI *uiPtr = static_cast<UI*>(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_LEFT)
        uiPtr->left_click(action);
    else if(button == GLFW_MOUSE_BUTTON_RIGHT)
        uiPtr->right_click(action);
}

