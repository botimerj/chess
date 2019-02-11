
#ifndef TILE_H
#define TILE_H

#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

#include <shader.h>
#include <texture.h>

class Tile{

public:
    unsigned int VAO;

    Tile();
    ~Tile();

    void draw(Shader &, Texture2D &, float size, glm::vec3 color, glm::vec2 pos);

};

#endif
