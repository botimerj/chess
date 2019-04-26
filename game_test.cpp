#include <iostream>
#include <game.h>


int main(){
    Game gs;
    
    std::string asdf;
    asdf = gs.gs_to_fen();

    std::cout << asdf << std::endl;

    return 0;
}
