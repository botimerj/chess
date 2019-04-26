#include <game.h>

Game::Game(){

    std::string ts_str("qkrnbpQKRNBP-");
    int i = 0;
    for(std::string::iterator e = ts_str.begin(); e != ts_str.end(); ++e){
        ts_to_char.insert(std::pair<TS, char>(static_cast<TS>(i), *e));
        char_to_ts.insert(std::pair<char, TS>(*e, static_cast<TS>(i++)));
    }


    //std::string default_pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    default_pos = std::string("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    //std::string default_pos("2kr2nr/1pp1p2p/p3Pppb/3p4/3P1B2/5NP1/PP2QP1P/nN3K1R w - - 1 15");

    set_board(default_pos);
}

void Game::set_board(std::string str_in){
    fen_to_gs(str_in);
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
    wk_castle = string_list[2].find("K") != std::string::npos;
    wq_castle = string_list[2].find("Q") != std::string::npos;
    bk_castle = string_list[2].find("k") != std::string::npos;
    bq_castle = string_list[2].find("q") != std::string::npos;

    // Set en passant 
    en_passant = string_list[3];

    // Set fifty move rule and number of turns
    fifty_move_rule = std::stoi(string_list[4], nullptr, 10);
    total_moves = std::stoi(string_list[5], nullptr, 10);

    // set board state
    std::string str = string_list[0];

    // convert all numbers to a series of '-' 
    std::size_t found = str.find_first_of("12345678");
    while(found !=std::string::npos){
        std::size_t n = atoi(str.substr(found,1).c_str());
        str.erase(found,1);
        str.insert(found, n, '-');
        found = str.find_first_of("12345678", found+n);
    }
    // erase all '/'
    found = str.find_first_of("/");
    while(found !=std::string::npos){
        str.erase(found,1);
        found = str.find_first_of("/", found);
    }

    // error checking
    if(str.length() != 64){
        std::cout << "bad fen string" << std::endl;
        return;
    }

    // convert to bstate
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            bstate[j][i] = char_to_ts[str[i*8 + j]];
        }
    }
}

std::string Game::gs_to_fen(){
    std::vector<std::string> string_list;    

    // Board state
    std::string state_string;
    char buf[2] = {' ','\0'};
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            buf[0] = ts_to_char[bstate[j][i]];
            state_string.append(std::string(buf));
        }
        state_string.append(std::string("/"));
    }
    state_string.erase(state_string.end()-1);
    std::size_t found = state_string.find("-");
    while(found != std::string::npos){
        state_string.erase(found,1);
        state_string.insert(found, std::to_string(1));
        if( found != 0 ){
            if( std::isdigit(state_string[found-1]) ){
                    int a = atoi(state_string.substr(found-1,1).c_str())+1;
                    state_string.erase(found-1,2);
                    state_string.insert(found-1, std::to_string(a));
            }
        }
        found = state_string.find("-");
    } 
    string_list.push_back(state_string);

    // Turn
    if(turn){ string_list.push_back(std::string("b ")); }
    else    { string_list.push_back(std::string("w ")); }

    // Castling 
    std::string castling;
    if(wk_castle){ castling.append(std::string("K")); } 
    if(wq_castle){ castling.append(std::string("Q")); } 
    if(bk_castle){ castling.append(std::string("k")); } 
    if(bq_castle){ castling.append(std::string("q")); } 
    if(castling.empty()){ castling.append(std::string("-")); }
    castling.append(std::string(" "));
    string_list.push_back(castling);

    // En Passant
    string_list.push_back(en_passant+std::string(" "));

    // Fifty move rule and total moves
    string_list.push_back(std::string(std::to_string(fifty_move_rule) + std::string(" ")));
    string_list.push_back(std::to_string(total_moves));

    std::string fen;
    for(std::vector<std::string>::iterator e = string_list.begin(); e != string_list.end(); ++e){
        fen.append(*e);
        //std::cout << *e << std::endl;
    }

    return fen;
}
