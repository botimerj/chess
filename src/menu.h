#ifndef MENU_H
#define MENU_H

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
#include <board.h> 

class Rectangle{
public:
    unsigned int VAO;
    Shader      *shader;
    Texture2D   *texture;

    Rectangle(Resource_manager *rm);
    void render(glm::vec3 color, glm::vec2 pos, glm::vec2 size);
};

class Triangle{
};

class Menu{
public:

    Rectangle * background;

    Menu(Resource_manager *rm);
    ~Menu();

    void render(float aspect_ratio);

};

#endif


