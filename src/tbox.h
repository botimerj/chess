#ifndef TBOX_H
#define TBOX_H

// Standard libraries
#include <map>
#include <string>
#include <iostream>

// Freetype
#include <ft2build.h>
#include FT_FREETYPE_H

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


struct Character{
    unsigned int VAO;       // Vertex attribute array
    unsigned int VBO;       // Vertex buffer object
    glm::vec2    Bearing;     // Offset from baseline to left/top of glyph
    float        Advance;     // Offset to advance to next glyph
};

class Text{
public: 
    unsigned int                texture;
    std::map<GLchar, Character> characters;
    unsigned int size;
    float        size_f;
    Shader *shader;

    Text(Resource_manager *rm);
    void resize_text(unsigned int size);
    void render(std::string str, glm::vec2 pos, float size);
};

class Tbox{
public:
   
    // Generate character textures
    Text *text;

    std::string sentence;
    glm::vec2 pos;
    float size;

    Tbox(Text *t); 
    void render();

};

#endif
