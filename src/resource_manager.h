#ifndef RM_H
#define RM_H

// Display libraries
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <shader.h>
#include <texture.h>

// Library for loading textures 
#include <stb_image/stb_image.h>

// This class is responsible for holding all the textures and shaders
// used throughout the program

class Resource_manager{
public:
    Shader    *shader;
    Texture2D *texture;

    Resource_manager();
    ~Resource_manager();
};

#endif
