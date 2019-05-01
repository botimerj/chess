#include <fstream>
#include <iostream>
#include <game.h>


int main(){
    Game gs;
    

    // open file, read fen string, and set board
    std::fstream f;
    f.open("fen");
    if(!f.is_open()){
        std::cout << "Error opening file" << std::endl;
        return 1;
    }
    char line[256];
    f.getline(line,256);
    gs.set_board(std::string(line));
    f.close();

    std::string out_str;
    out_str = gs.gs_to_fen();

    std::cout << out_str << std::endl;

    return 0;
}
