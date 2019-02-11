#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <iostream>
#include <stdio.h> 
#include <stdlib.h>
#include <cstdlib>
#include <vector>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

#include <shader.h>
#include <texture.h>
#include <tile.h>
#include <piece.h>

#include <stb_image/stb_image.h>

enum Board{
    b_queen,
    b_king,
    b_rook,
    b_knight,
    b_bishop,
    b_pawn,
    w_queen,
    w_king,
    w_rook,
    w_knight,
    w_bishop,
    w_pawn,
    empty 
};

enum Color{
    black,
    white,
    none

};

class GameState{
public:
    // Drawing things
    Shader    *shaders;
    Texture2D *textures;

    // Display
    GLFWwindow *window;
    int WIDTH;
    int HEIGHT;
    float aspect_ratio;
    glm::mat4 projection;
    glm::vec2 border;
    float tile_size;
    glm::vec3 color_arr[4];
    Tile tile;
    std::vector<Piece> pieces;

    // Mouse
    glm::vec2 mouse_pos;


    // Elements
    // Player that is to move next
    Color turn;                  
    // Complete state of chess board
    Board bstate[8][8];         
    // The current selected piece and index
    Board selected;
    glm::vec2 selected_idx;
    glm::vec2 check_idx;

    // Last turns play
    glm::vec2 played_from_idx;
    glm::vec2 played_idx;

    // Castling variables
    bool b_kside_castle;
    bool b_qside_castle;
    bool w_kside_castle;
    bool w_qside_castle;

    // Gamestate functions
    GameState(GLFWwindow *window, int WIDTH, int HEIGHT); 
    ~GameState();

    void render();
    void processInput();
    void resizeWindow(int width, int height);
    void leftClick(int action);
    void getBoardIdx(int *xidx, int *yidx);
    void getMousePos();
    void rules(glm::vec2 new_idx);
    bool movement(glm::vec2 new_idx, bool *en_passant, 
                                     bool *castle, 
                                     bool *promote);
    Color piece_color(Board piece);
    std::vector<glm::vec2> tile_attackers(glm::vec2 idx, 
                                          Color color);


    // For testing
    std::vector <glm::vec2> selected_attackers;

};



#endif
