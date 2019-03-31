
#include <iostream> 
#include <cstdlib>
#include <string>
#include <map>


enum TS{
    q,k,r,n,b,p,
    Q,K,R,N,B,P,
    e
};

std::string preprocess(std::string fen){
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

int main(){

    std::string ts_str("qkrnbpQKRNBP-");
    std::map<TS, char> ts_to_char;
    std::map<char, TS> char_to_ts;
    int i = 0;
    for(std::string::iterator e = ts_str.begin(); e != ts_str.end(); ++e){
        ts_to_char.insert(std::pair<TS, char>(static_cast<TS>(i++), *e));
        char_to_ts.insert(std::pair<char, TS>(*e, static_cast<TS>(i++)));
    }

    TS bstate[8][8];
    std::string str_in("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    while(str_in.compare("exit")){
        std::getline(std::cin, str_in);
        // Print board
        std::string fen_pos = preprocess(str_in);
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++){
                bstate[j][i] = char_to_ts[fen_pos[i*8 + j]];
                printf("%c ", fen_pos[i*8 + j]);
            }
            printf("\n");
        }
    }

    return 0;
}



