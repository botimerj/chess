#ifndef UI_H
#define UI_H

#include <vector>
#include <iostream>

// Display libraries
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <shader.h>
#include <texture.h>

// GLM libraries
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include <glm/geometric.hpp>
//#include <glm/trigonometric.hpp>

// Library for loading textures 
#include <stb_image/stb_image.h>

#include <tile.h>


class Partition{
public:

    Partition();
    ~Partition();

    glm::vec2   dimension;
    virtual void render() { std::cout << "Render is not defined for general partition" << std::endl; }
};

class Resource_manager{
public:
    Shader    *shader;
    Texture2D *texture;

    Resource_manager();
    ~Resource_manager();

};

//class Box : public Partition{
class Box{
public:
    unsigned int VAO;
    Shader      *shader;
    Texture2D   *texture;

    glm::vec3 color;
    glm::vec2 dimension;

    Box(Resource_manager *rm, glm::vec2 dimension, glm::vec3 color);
    ~Box();

    void render(glm::vec2 coor);
    
};

// This class defines the user interface.
// The UI is made up of partitions which are responsible for hosting elements
class UI{
public:

    // Display
    GLFWwindow *window;
    int WIDTH;
    int HEIGHT;
    float aspect_ratio;
    glm::mat4 projection;
    Resource_manager * rm;

    //Shader    *shader;
    //Texture2D *texture;

    // Mouse
    glm::vec2 mouse_pos;

    // Partitions
    //std::vector<Partition *> partition;
    //std::vector<Box> box;
    //Box *box;
    Tile tile;
    std::vector<glm::vec2> coor;

    // UI Functions
    UI(GLFWwindow *window, int WIDTH, int HEIGHT); 
    ~UI();
    void resize_window(int width, int height);
    void render();
    

};

#endif
