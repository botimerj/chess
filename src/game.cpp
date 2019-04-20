#include <game.h>

Game::Game(){

    std::string ts_str("qkrnbpQKRNBP-");
    int i = 0;
    for(std::string::iterator e = ts_str.begin(); e != ts_str.end(); ++e){
        ts_to_char.insert(std::pair<TS, char>(static_cast<TS>(i), *e));
        char_to_ts.insert(std::pair<char, TS>(*e, static_cast<TS>(i++)));
    }


    std::string default_pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    //std::string default_pos("2kr2nr/1pp1p2p/p3Pppb/3p4/3P1B2/5NP1/PP2QP1P/nN3K1R w - - 1 15");

    set_board(default_pos);
}

void Game::set_board(std::string str_in){
    //std::string fen_pos = preprocess(str_in);
    fen_to_gs(str_in);
    //if(fen_pos.length() != 64){
    //    std::cout << "bad fen string" << std::endl;
    //    return;
    //}
}


void Game::fen_to_gs(std::string fen){

    // Parse fen string into components
    std::vector<std::string> string_list;
    std::size_t lower = 0;
    std::size_t upper = 0;

    while(lower != std::string::npos){
        upper = fen.find(" ");
        string_list.push_back(fen.substr(0,upper));
        fen = fen.substr(upper+1, std::string::npos);
        lower = upper;
    }

    // Set turn
    turn = "w" == string_list[1];

    // Set castling
    w_castle = 0;
    b_castle = 0;

    if(string_list[2].find("K") != std::string::npos){
        w_castle = 2;
    }
    if(string_list[2].find("Q") != std::string::npos){
        w_castle += 1;
    }
    if(string_list[2].find("k") != std::string::npos){
        b_castle = 2;
    }
    if(string_list[2].find("q") != std::string::npos){
        b_castle += 1;
    }

    // Set en passant 
    en_passant = string_list[3];

    // Set fifty move rule and number of turns
    fifty_move_rule = std::stoi(string_list[4], nullptr, 10);
    total_moves = std::stoi(string_list[5], nullptr, 10);

    // Set board state
    std::string str = string_list[0];

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

    // Error checking
    if(str.length() != 64){
        std::cout << "bad fen string" << std::endl;
        return;
    }

    // Convert to bstate
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            //bstate[j][i] = char_to_ts[fen_pos[i*8 + j]];
            bstate[j][i] = char_to_ts[str[i*8 + j]];
        }
    }
}

//void Game::gs_to_fen(){
//    
//
//}
