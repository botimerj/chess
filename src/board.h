#ifndef BOARD_H
#define BOARD_H

// Standard libraries
#include <vector>

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

//Tile class for drawing board tiles
class Tile{
public:
    unsigned int VAO;
    Shader      *shader;
    Texture2D   *texture;

    Tile(Resource_manager *rm);
    void render(glm::vec3 color, glm::vec2 pos, float size);
};

// Piece class for drawing board pieces
class Piece{
public:
    unsigned int VAO;
    Shader      *shader;
    Texture2D   *texture;

    Piece(Resource_manager *rm, int x_offset, int y_offset);
    void render(glm::vec2 pos, float size);
};

// Tile state
enum TS{
    q,k,r,n,b,p,
    Q,K,R,N,B,P,
    e 
};

// Tile color 
enum TC{
    white,
    black,
    highlight,
    tile_select,
    played_from,
    check,
    none
};

// Board class responsible for all things board 
class Board{
public:
    // An array of TS to depict current board state 
    TS bstate[8][8];         
    TC bcolor[8][8];

    // Board display characteristics
    glm::vec2 coor;
    float dim;

    glm::vec3 color_arr[7];

    // Board state
    TS selected;
    glm::vec2 higlight_idx;
    glm::vec2 check_idx;
    glm::vec2 selected_idx;
    glm::vec2 played_from_idx;


    Tile * tile;
    std::vector<Piece> pieces;

    Board(Resource_manager *rm);
    ~Board();

    void render(glm::vec2 mpos, float aspect_ratio);

    // Clicking the board 
    void board_right_click(glm::vec2 mpos);
    void board_left_click(glm::vec2 mpos, bool down);
    void get_board_idx(glm::vec2 mpos, int *xidx, int *yidx);
};

#endif
