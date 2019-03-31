#ifndef GAME_H
#define GAME_H

#include <board.h>

// Tile state
enum TS{
    q,k,r,n,b,p,
    Q,K,R,N,B,P,
    e 
};

class Game{
public:

    TS bstate[8][8];         
    bool w_k_castle;
    bool w_q_castle;

};



#endif
