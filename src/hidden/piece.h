
#ifndef PIECE_H
#define PIECE_H

#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
//#include <glm/gtc/type_ptr.hpp>
#include <cmath>

#include <shader.h>
#include <texture.h>

class Piece{

public:
    unsigned int VAO;
    // Shape and physics
    glm::vec3 color;

    Piece(int x_offset, int y_offset);
    ~Piece();

    void draw(Shader &, Texture2D &, float size, glm::vec2 pos);
};

#endif
