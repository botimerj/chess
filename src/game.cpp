#include <game.h>

///////////////////////////
/// Game State Functions //
///////////////////////////

Game::Game(){

    std::string ts_str("qkrnbpQKRNBP-");
    int i = 0;
    for(std::string::iterator e = ts_str.begin(); e != ts_str.end(); ++e){
        ts_to_char.insert(std::pair<TS, char>(static_cast<TS>(i), *e));
        char_to_ts.insert(std::pair<char, TS>(*e, static_cast<TS>(i++)));
    }

    default_pos = std::string("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    set_board(default_pos);
}

void Game::set_board(std::string str_in){
    fen_to_gs(str_in);
    result = -1;
    last_move = std::string("-");
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
    turn = !("w" == string_list[1]);

    // Set castling
    wk_castle = string_list[2].find("K") != std::string::npos;
    wq_castle = string_list[2].find("Q") != std::string::npos;
    bk_castle = string_list[2].find("k") != std::string::npos;
    bq_castle = string_list[2].find("q") != std::string::npos;

    // Set en passant 
    std::string ep = string_list[3];
    if(ep == "-") en_passant = glm::ivec2(-1, -1);
    else          en_passant = glm::ivec2(ep[0]-'a', 7-(ep[1]-'0'));
    //std::cout << "(" << en_passant.x << "," << en_passant.y << ")" << std::endl;
    //en_passant = string_list[3];

    // Set fifty move rule and number of turns
    fifty_move_rule = std::stoi(string_list[4], nullptr, 10);
    onehundred_move_rule = fifty_move_rule*2;
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
    if(en_passant.x == -1 || en_passant.y == -1){
        string_list.push_back(std::string("- "));
    }else{
        char tmp[3];
        tmp[0] = en_passant.x + 'a';
        tmp[1] = 7-en_passant.y + '0';
        tmp[2] = '\0';
        string_list.push_back(std::string(tmp) + std::string(" "));
    }

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

////////////////////////
// Movement Functions //
////////////////////////

bool Game::move(TS sel, glm::ivec2 to, glm::ivec2 from){
    if(result != -1){
        std::cout << "Result: " << result << ". Start another game." << std::endl;
    }

    // Check same square
    bool same_square = (to.x == from.x) && (to.y == from.y);

    // Check turn
    bool turn_order = ts_color(sel)==turn;
    
    // Check rules
    std::list<glm::ivec2> move_list = valid_moves(sel, from);
    bool legal_move = std::find(move_list.begin(), move_list.end(), to) != move_list.end();

    //bool valid_move = !same_square && turn_order && legal_move;

    // Perform move 
    if(!same_square && turn_order && legal_move){
        // Update move count when blacks move finishes
        if(turn==1){
            total_moves += 1;
        }

        // Update turn
        turn = !turn;

        // Update castling 
        if(sel == TS::R || sel == TS::K){
            if((from.x==4 || from.x==7) && from.y==7) wk_castle = false;
            if((from.x==4 || from.x==0) && from.y==7) wq_castle = false;
        }
        if(sel == TS::r || sel == TS::k){
            if((from.x==4 || from.x==7) && from.y==0) bk_castle = false;
            if((from.x==4 || from.x==0) && from.y==0) bq_castle = false;
        }

        // Update fifty_move_rule
        if( sel==TS::p || sel==TS::P || opposite_piece(to,from) ) 
            onehundred_move_rule = 0;
        else
            onehundred_move_rule += 1;
        fifty_move_rule = static_cast<int>(onehundred_move_rule/2);

        // Update board
        bstate[from.x][from.y] = TS::e;
        bstate[to.x][to.y] = sel;

        // Special cases
        if(sel==TS::K && (to.x-from.x)==2){ bstate[5][7]=TS::R; bstate[7][7]=TS::e; }
        if(sel==TS::K && (from.x-to.x)==2){ bstate[3][7]=TS::R; bstate[0][7]=TS::e; }
        if(sel==TS::k && (to.x-from.x)==2){ bstate[5][0]=TS::r; bstate[7][0]=TS::e; }
        if(sel==TS::k && (from.x-to.x)==2){ bstate[3][0]=TS::r; bstate[0][0]=TS::e; }

        if(to.x==en_passant.x && to.y==en_passant.y) {
            if(sel==TS::P) bstate[to.x][3]=TS::e;
            if(sel==TS::p) bstate[to.x][4]=TS::e;
        }
        
        // Promotion not handled correctly
        if(sel==TS::P && to.y==0) bstate[to.x][to.y] = TS::Q;
        if(sel==TS::p && to.y==7) bstate[to.x][to.y] = TS::q;
        
        
        // Update en passant
        if(sel == TS::P && (from.y-to.y)==2)
            en_passant = glm::ivec2(from.x, from.y-1);
        else if(sel == TS::p && (to.y-from.y)==2)
            en_passant = glm::ivec2(from.x, from.y+1);
        else
            en_passant = glm::ivec2(-1, -1);

        // Check winning condition
        bool has_valid_moves = false;
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++){
                if(ts_color(bstate[i][j]) == turn){
                    if(!valid_moves(bstate[i][j], glm::ivec2(i,j)).empty()){
                        has_valid_moves = true;
                        break;
                    }
                }
            }
        }

        if(!has_valid_moves){ // no valid moves for next player
            glm::ivec2 k_idx = king_idx(turn);
            if(tile_attacked(k_idx, !turn)) result = !turn; //checkmate
            else                            result = 2; // stalemate
        }
        if(onehundred_move_rule >= 100){
            result = 2; //stalemate
        }

        last_move = vec_to_move_str(to, from);

        return true;
    }
    else{
        return false;
    }
}

std::list<glm::ivec2> Game::valid_moves(TS sel, glm::ivec2 from){

    bool wk_castle_tmp = wk_castle;          
    bool wq_castle_tmp = wq_castle;         
    bool bk_castle_tmp = bk_castle;
    bool bq_castle_tmp = bq_castle;

    // Temp castling variables
    if( tile_attacked(glm::ivec2(5,7), 1) ||  
        tile_attacked(glm::ivec2(6,7), 1)) wk_castle = false;
    if( tile_attacked(glm::ivec2(2,7), 1) ||  
        tile_attacked(glm::ivec2(3,7), 1)) wq_castle = false;
    if( tile_attacked(glm::ivec2(5,0), 0) ||  
        tile_attacked(glm::ivec2(6,0), 0)) bk_castle = false;
    if( tile_attacked(glm::ivec2(2,0), 0) ||  
        tile_attacked(glm::ivec2(3,0), 0)) bq_castle = false;

    std::list<glm::ivec2> move_list = candidate_moves(sel, from);
    std::list<glm::ivec2> valid_move_list;

    glm::ivec2 k_idx;
    for(std::list<glm::ivec2>::iterator it = move_list.begin(); it != move_list.end(); ++it){
        TS tmp_ts = bstate[(*it).x][(*it).y];
        bstate[(*it).x][(*it).y] = sel;
        bstate[from.x][from.y] = TS::e;
        k_idx = king_idx(ts_color(sel));
        if(!tile_attacked(k_idx, !ts_color(sel))){
            valid_move_list.push_back(*it);
        }
        bstate[from.x][from.y] = sel;
        bstate[it->x][it->y] = tmp_ts;
    }
    

    wk_castle = wk_castle_tmp;          
    wq_castle = wq_castle_tmp;         
    bk_castle = bk_castle_tmp;
    bq_castle = bq_castle_tmp;

    return valid_move_list;
}

std::list<glm::ivec2> Game::candidate_moves(TS sel, glm::ivec2 from){
    std::list<glm::ivec2> move_list;

    switch (sel){
        case TS::q: case TS::Q:
            move_list = queen(from);
            break;
        case TS::k: case TS::K:
            move_list = king(from);
            break;
        case TS::r: case TS::R:
            move_list = rook(from);
            break;
        case TS::n: case TS::N:
            move_list = knight(from);
            break;
        case TS::b: case TS::B:
            move_list = bishop(from);
            break;
        case TS::p: case TS::P:
            move_list = pawn(from);
            break;
        default: 
            break;
    }

    return move_list;
}

std::list<glm::ivec2> Game::pawn(glm::ivec2 from){
    std::list<glm::ivec2> move_list;

    // White moves 'negative' and Black moves 'positive'
    int mod = (static_cast<int>(bstate[from.x][from.y]) == static_cast<int>(TS::p)) ? 1 : -1;

    if( in_bounds(from.x,from.y+mod) && read_bstate(from.x,from.y+mod) == TS::e ){
        // Move forward one square as long as its empty
        move_list.push_back(glm::ivec2(from.x,from.y+mod));
        if( (read_bstate(from.x,from.y+mod+mod) == TS::e) && 
            ((mod==-1 && from.y==6) || (mod==1 && from.y==1)) )
                // Move forward two squares as long as its empty and its the inital move
                move_list.push_back(glm::ivec2(from.x,from.y+mod+mod));
    }

    // Take diagonally if opposing piece is there and its in bounds
    glm::ivec2 idx(from.x+1, from.y+mod);
    if(opposite_piece(idx, from) && in_bounds(idx.x, idx.y) )
        move_list.push_back(idx);

    idx = glm::ivec2(from.x-1, from.y+mod);
    if(opposite_piece(idx, from) && in_bounds(idx.x, idx.y) )
        move_list.push_back(idx);


    // Take en_passant if available
    if(from.x+1 == en_passant.x && from.y+mod == en_passant.y)
        move_list.push_back(glm::ivec2(from.x+1,from.y+mod));
    if(from.x-1 == en_passant.x && from.y+mod == en_passant.y)
        move_list.push_back(glm::ivec2(from.x-1,from.y+mod));

    return move_list;
}

std::list<glm::ivec2> Game::bishop(glm::ivec2 from){
    std::list<glm::ivec2> move_list;

    std::list<glm::ivec2> mod_list;
    mod_list.push_back(glm::ivec2( 1,  1));
    mod_list.push_back(glm::ivec2(-1,  1));
    mod_list.push_back(glm::ivec2( 1, -1));
    mod_list.push_back(glm::ivec2(-1, -1));

    for(std::list<glm::ivec2>::iterator it = mod_list.begin(); it != mod_list.end(); ++it){
        glm::ivec2 mod = *it;
        glm::ivec2 idx(from.x+mod.x, from.y+mod.y);
        while(read_bstate(idx.x, idx.y) == TS::e && in_bounds(idx.x,idx.y)){
            move_list.push_back(idx);
            idx = glm::ivec2(idx.x+mod.x, idx.y+mod.y);
        }
        if( opposite_piece(idx, from) ){ move_list.push_back(idx); }
    }

    return move_list;
}

std::list<glm::ivec2> Game::rook(glm::ivec2 from){
    std::list<glm::ivec2> move_list;

    std::list<glm::ivec2> mod_list;
    mod_list.push_back(glm::ivec2( 0,  1));
    mod_list.push_back(glm::ivec2( 0, -1));
    mod_list.push_back(glm::ivec2( 1,  0));
    mod_list.push_back(glm::ivec2(-1,  0));

    for(std::list<glm::ivec2>::iterator it = mod_list.begin(); it != mod_list.end(); ++it){
        glm::ivec2 mod = *it;
        glm::ivec2 idx(from.x+mod.x, from.y+mod.y);
        while(read_bstate(idx.x, idx.y) == TS::e && in_bounds(idx.x,idx.y)){
            move_list.push_back(idx);
            idx = glm::ivec2(idx.x+mod.x, idx.y+mod.y);
        }
        if( opposite_piece(idx, from) ){ move_list.push_back(idx); }
    }
    return move_list;
}

std::list<glm::ivec2> Game::queen(glm::ivec2 from){
    std::list<glm::ivec2> move_list;

    std::list<glm::ivec2> mod_list;
    mod_list.push_back(glm::ivec2( 1,  1));
    mod_list.push_back(glm::ivec2(-1,  1));
    mod_list.push_back(glm::ivec2( 1, -1));
    mod_list.push_back(glm::ivec2(-1, -1));
    mod_list.push_back(glm::ivec2( 0,  1));
    mod_list.push_back(glm::ivec2( 0, -1));
    mod_list.push_back(glm::ivec2( 1,  0));
    mod_list.push_back(glm::ivec2(-1,  0));

    for(std::list<glm::ivec2>::iterator it = mod_list.begin(); it != mod_list.end(); ++it){
        glm::ivec2 mod = *it;
        glm::ivec2 idx(from.x+mod.x, from.y+mod.y);
        while(read_bstate(idx.x, idx.y) == TS::e && in_bounds(idx.x,idx.y)){
            move_list.push_back(idx);
            idx = glm::ivec2(idx.x+mod.x, idx.y+mod.y);
        }
        if( opposite_piece(idx, from) ){ move_list.push_back(idx); }
    }
    return move_list;
}

std::list<glm::ivec2> Game::knight(glm::ivec2 from){
    std::list<glm::ivec2> move_list;

    std::list<glm::ivec2> mod_list;
    mod_list.push_back(glm::ivec2( 1,  2));
    mod_list.push_back(glm::ivec2( 2,  1));
    mod_list.push_back(glm::ivec2(-1,  2));
    mod_list.push_back(glm::ivec2(-2,  1));
    mod_list.push_back(glm::ivec2( 1, -2));
    mod_list.push_back(glm::ivec2( 2, -1));
    mod_list.push_back(glm::ivec2(-1, -2));
    mod_list.push_back(glm::ivec2(-2, -1));

    for(std::list<glm::ivec2>::iterator it = mod_list.begin(); it != mod_list.end(); ++it){
        glm::ivec2 mod = *it;
        glm::ivec2 idx(from.x+mod.x, from.y+mod.y);
        if((read_bstate(idx.x, idx.y)==TS::e || opposite_piece(idx, from)) && in_bounds(idx.x,idx.y)){
            move_list.push_back(idx);
        }
    }
    return move_list;
}

std::list<glm::ivec2> Game::king(glm::ivec2 from){
    std::list<glm::ivec2> move_list;

    std::list<glm::ivec2> mod_list;
    mod_list.push_back(glm::ivec2(-1, -1));
    mod_list.push_back(glm::ivec2(-1,  0));
    mod_list.push_back(glm::ivec2(-1,  1));
    mod_list.push_back(glm::ivec2( 0, -1));
    mod_list.push_back(glm::ivec2( 0,  1));
    mod_list.push_back(glm::ivec2( 1, -1));
    mod_list.push_back(glm::ivec2( 1,  0));
    mod_list.push_back(glm::ivec2( 1,  1));

    for(std::list<glm::ivec2>::iterator it = mod_list.begin(); it != mod_list.end(); ++it){
        glm::ivec2 mod = *it;
        glm::ivec2 idx(from.x+mod.x, from.y+mod.y);
        if((read_bstate(idx.x, idx.y)==TS::e || opposite_piece(idx, from)) && in_bounds(idx.x,idx.y)){
            move_list.push_back(idx);
        }
    }
    
    // Castling 
    glm::ivec2 idx(from.x, from.y);
    if(ts_color(read_bstate(from.x,from.y))==0){
        if(wk_castle && bstate[5][7]==TS::e && 
                        bstate[6][7]==TS::e) move_list.push_back(glm::ivec2(idx.x+2,idx.y));
        if(wq_castle && bstate[3][7]==TS::e &&
                        bstate[2][7]==TS::e && 
                        bstate[1][7]==TS::e) move_list.push_back(glm::ivec2(idx.x-2,idx.y));
    }else if(ts_color(read_bstate(from.x,from.y))==1){
        if(bk_castle && bstate[5][0]==TS::e && 
                        bstate[6][0]==TS::e) move_list.push_back(glm::ivec2(idx.x+2,idx.y));
        if(bq_castle && bstate[3][0]==TS::e &&
                        bstate[2][0]==TS::e && 
                        bstate[1][0]==TS::e) move_list.push_back(glm::ivec2(idx.x-2,idx.y));
    }

    return move_list;
}

bool Game::tile_attacked(glm::ivec2 tile, int color){
    TS tmp_ts = bstate[tile.x][tile.y];
    if(color == 0) // White
        bstate[tile.x][tile.y] = TS::p; // Put a black pawn to sample if it is attacked
    if(color == 1) 
        bstate[tile.x][tile.y] = TS::P;

    bool attacked = false;
    TS sel;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            sel = bstate[i][j];
            if(sel == TS::e || (tile.x==i && tile.y==j)){ continue; }
            if(ts_color(sel) == color){ 
                std::list<glm::ivec2> move_list = candidate_moves(sel, glm::ivec2(i,j));
                if(std::find(move_list.begin(), move_list.end(), tile) != move_list.end())
                    attacked = true;
            }
        }
    }

    // Reset bstate
    bstate[tile.x][tile.y] = tmp_ts;
    return attacked;
}

// Helper functions
glm::ivec2 Game::king_idx(int color){
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(color == 0 && bstate[i][j] == TS::K)
                return glm::ivec2(i,j);
            if(color == 1 && bstate[i][j] == TS::k)
                return glm::ivec2(i,j);
        }
    }
    return glm::ivec2(-1,-1);
}

bool Game::opposite_piece(glm::ivec2 idx0, glm::ivec2 idx1){
    return (ts_color(read_bstate(idx0.x,idx0.y))==0 && ts_color(read_bstate(idx1.x,idx1.y))==1) ||
           (ts_color(read_bstate(idx0.x,idx0.y))==1 && ts_color(read_bstate(idx1.x,idx1.y))==0);
}

int Game::ts_color(TS tile){
    int tile_int = static_cast<int> (tile);
    if     (tile_int < 6){ return 1; }
    else if(tile_int >= 6 && tile_int < 12){ return 0; }
    else{ return 2; }
}

TS Game::read_bstate(int x, int y){
    // Out of bounds
    if( !in_bounds(x,y) ) {return TS::e;}
    else {return bstate[x][y];}
}

bool Game::in_bounds(int x, int y){
    return x >=0 && x < 8 && y >= 0 && y < 8;
}

std::string Game::vec_to_move_str(glm::ivec2 to, glm::ivec2 from){

    char move_str[5];
    move_str[0] = from.x+'a';
    move_str[1] = (8-from.y)+'0';
    move_str[2] = to.x+'a';
    move_str[3] = (8-to.y)+'0';
    move_str[4] = '\0';

    return std::string(move_str); 
}
