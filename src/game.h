#ifndef GAME_H
#define GAME_H

#include <string> 
#include <map>

#include <board.h>


class Game{
public:

    std::map<TS, char> ts_to_char;
    std::map<char, TS> char_to_ts;

    // gamestate variables
    TS bstate[8][8];         
    bool turn;              // 0 - white; 1 - black
    int  w_castle;          // 2'b11 => kq 
    int  b_castle;
    int  fifty_move_rule;
    int  total_moves;
    // Some way to hold 3 move repeat // 



    Game();
    void set_board(std::string str_in);
    //std::string fen_to_gs(std::string fen);
    std::string preprocess(std::string fen);

};

#endif

