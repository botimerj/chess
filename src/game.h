#ifndef GAME_H
#define GAME_H

// Standard Libraries
#include <iostream>
#include <string> 
#include <algorithm> 
#include <vector> 
#include <list> 
#include <map>

// GLM libraries
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Custom libraries
//#include <board.h>

// Tile state
enum TS{
    q,k,r,n,b,p,
    Q,K,R,N,B,P,
    e 
};

class Game{
public:

    std::map<TS, char> ts_to_char;
    std::map<char, TS> char_to_ts;

    // gamestate variables
    int result; // -1 - game is in progress | 0 - white wins | 1 - black wins | 2 - stalemate
    TS bstate[8][8];         
    bool turn;              // 0 - white; 1 - black
    bool wk_castle;          
    bool wq_castle;         
    bool bk_castle;
    bool bq_castle;
    glm::ivec2 en_passant;
    int  fifty_move_rule;
    int  onehundred_move_rule;
    int  total_moves;
    std::string last_move;
    // Some way to hold 3 move repeat 

    std::string default_pos;


    Game();
    void set_board(std::string str_in);
    void fen_to_gs(std::string fen);
    std::string gs_to_fen();

    // Move calculations
    bool move(TS sel, glm::ivec2 to, glm::ivec2 from);
    std::list<glm::ivec2> valid_moves(TS sel, glm::ivec2 from);
    std::list<glm::ivec2> candidate_moves(TS sel, glm::ivec2 from);
    std::list<glm::ivec2> pawn   (glm::ivec2 from);
    std::list<glm::ivec2> bishop (glm::ivec2 from);
    std::list<glm::ivec2> rook   (glm::ivec2 from);
    std::list<glm::ivec2> queen  (glm::ivec2 from);
    std::list<glm::ivec2> knight (glm::ivec2 from);
    std::list<glm::ivec2> king   (glm::ivec2 from);
    bool tile_attacked(glm::ivec2 tile, int color);

    // Helper functions
    glm::ivec2 king_idx(int color);
    int ts_color(TS tile);
    bool opposite_piece(glm::ivec2 idx0, glm::ivec2 idx1);
    TS read_bstate(int x, int y);
    bool in_bounds(int x, int y);
    std::string vec_to_move_str(glm::ivec2 to, glm::ivec2 from);
};

#endif

