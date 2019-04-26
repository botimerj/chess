#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <string> 
#include <vector> 
#include <map>

#include <board.h>


class Game{
public:

    std::map<TS, char> ts_to_char;
    std::map<char, TS> char_to_ts;

    // gamestate variables
    TS bstate[8][8];         
    bool turn;              // 0 - white; 1 - black
    bool wk_castle;          
    bool wq_castle;         
    bool bk_castle;
    bool bq_castle;
    std::string en_passant;
    int  fifty_move_rule;
    int  total_moves;
    // Some way to hold 3 move repeat // 

    std::string default_pos;


    Game();
    void set_board(std::string str_in);
    //std::string fen_to_gs(std::string fen);
    void fen_to_gs(std::string fen);
    std::string gs_to_fen();

};

#endif

