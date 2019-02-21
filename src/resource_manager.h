#ifndef RM_H
#define RM_H

// Standard Libraries
#include <string> 

// Display libraries
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <shader.h>
#include <texture.h>

// Library for loading textures 
#include <stb_image/stb_image.h>

// Custom libraries
#include <tbox.h>

// This class is responsible for holding all the textures and shaders
// used throughout the program

class Resource_manager{
public:
    Shader    *shader;
    Texture2D *texture;

    Text       *text;
    Text       *textb;
    Text       *texti;
    Text       *textbi;

    Resource_manager(int width);
    ~Resource_manager();
};

#endif
