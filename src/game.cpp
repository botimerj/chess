#include <game.h>

Game::Game(){

    std::string ts_str("qkrnbpQKRNBP-");
    int i = 0;
    for(std::string::iterator e = ts_str.begin(); e != ts_str.end(); ++e){
        ts_to_char.insert(std::pair<TS, char>(static_cast<TS>(i), *e));
        char_to_ts.insert(std::pair<char, TS>(*e, static_cast<TS>(i++)));
    }


    //std::string default_pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    std::string default_pos("2kr2nr/1pp1p2p/p3Pppb/3p4/3P1B2/5NP1/PP2QP1P/nN3K1R w - - 1 15");

    set_board(default_pos);
}

void Game::set_board(std::string str_in){
    std::string fen_pos = preprocess(str_in);
    //if(fen_pos.length() != 64){
    //    std::cout << "bad fen string" << std::endl;
    //    return;
    //}
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            bstate[j][i] = char_to_ts[fen_pos[i*8 + j]];
        }
    }
}


std::string Game::preprocess(std::string fen){
    // Take only the board part of FEN string 
    std::string str = fen.substr(0,fen.find(" "));
    // Convert all numbers to a series of '-' 
    std::size_t found = str.find_first_of("12345678");
    while(found !=std::string::npos){
        std::size_t n = atoi(str.substr(found,1).c_str());
        str.erase(found,1);
        str.insert(found, n, '-');
        found = str.find_first_of("12345678", found+n);
    }
    // Erase all '/'
    found = str.find_first_of("/");
    while(found !=std::string::npos){
        str.erase(found,1);
        found = str.find_first_of("/", found);
    }
    return str;
}

