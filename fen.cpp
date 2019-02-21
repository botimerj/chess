
#include <iostream> 
#include <cstdlib>
#include <string>
#include <map>


enum TS{
    q,k,r,n,b,p,
    Q,K,R,N,B,P,
    e
};

std::string preprocess(std::string str){
    std::string tmp = str.substr(0,str.find(" "));
    std::size_t found = tmp.find_first_of("12345678");
    while(found !=std::string::npos){
        std::size_t n = atoi(tmp.substr(found,1).c_str());
        tmp.insert(found, n, '-');
        //printf("%c, %d\n", tmp[found], atoi(tmp.substr(found,1).c_str()));
        found = tmp.find_first_of("12345678", found+1+n);
    }
    return tmp;
}

int main(){


    std::map<TS, char> ts_to_char;
    ts_to_char.insert(std::pair<TS, char>(TS::e, '-')); 
    ts_to_char.insert(std::pair<TS, char>(TS::q, 'q')); 
    ts_to_char.insert(std::pair<TS, char>(TS::k, 'k')); 
    ts_to_char.insert(std::pair<TS, char>(TS::r, 'r')); 
    ts_to_char.insert(std::pair<TS, char>(TS::n, 'n')); 
    ts_to_char.insert(std::pair<TS, char>(TS::b, 'b')); 
    ts_to_char.insert(std::pair<TS, char>(TS::p, 'p')); 
    ts_to_char.insert(std::pair<TS, char>(TS::Q, 'Q')); 
    ts_to_char.insert(std::pair<TS, char>(TS::K, 'K')); 
    ts_to_char.insert(std::pair<TS, char>(TS::R, 'R')); 
    ts_to_char.insert(std::pair<TS, char>(TS::N, 'N')); 
    ts_to_char.insert(std::pair<TS, char>(TS::B, 'B')); 
    ts_to_char.insert(std::pair<TS, char>(TS::P, 'P')); 

    std::map<char, TS> char_to_ts;
    char_to_ts.insert(std::pair<char, TS>('-', TS::e)); 
    char_to_ts.insert(std::pair<char, TS>('q', TS::q)); 
    char_to_ts.insert(std::pair<char, TS>('k', TS::k)); 
    char_to_ts.insert(std::pair<char, TS>('r', TS::r)); 
    char_to_ts.insert(std::pair<char, TS>('n', TS::n)); 
    char_to_ts.insert(std::pair<char, TS>('b', TS::b)); 
    char_to_ts.insert(std::pair<char, TS>('p', TS::p)); 
    char_to_ts.insert(std::pair<char, TS>('Q', TS::Q)); 
    char_to_ts.insert(std::pair<char, TS>('K', TS::K)); 
    char_to_ts.insert(std::pair<char, TS>('R', TS::R)); 
    char_to_ts.insert(std::pair<char, TS>('N', TS::N)); 
    char_to_ts.insert(std::pair<char, TS>('B', TS::B)); 
    char_to_ts.insert(std::pair<char, TS>('P', TS::P)); 

    TS bstate[8][8];

    for(int y = 0; y < 8; y++){
        for(int x = 0; x < 8; x++){
            bstate[x][y] = TS::e; // empty
        }
    }

    bstate[0][0] = TS::r;
    bstate[1][0] = TS::n;
    bstate[2][0] = TS::b;
    bstate[3][0] = TS::q;
    bstate[4][0] = TS::k;
    bstate[5][0] = TS::b;
    bstate[6][0] = TS::n;
    bstate[7][0] = TS::r;

    bstate[0][7] = TS::R;
    bstate[1][7] = TS::N;
    bstate[2][7] = TS::B;
    bstate[3][7] = TS::Q;
    bstate[4][7] = TS::K;
    bstate[5][7] = TS::B;
    bstate[6][7] = TS::N;
    bstate[7][7] = TS::R;

    for(int i = 0; i < 8; i++){
        bstate[i][1] = TS::p;
        bstate[i][6] = TS::P;
    }

    // Print board
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            char c = ts_to_char[bstate[j][i]];
            printf("%c ", c);
        }
        printf("\n");
    }

    std::string str_in;
    while(str_in.compare("exit")){
        std::getline(std::cin, str_in);
        printf("preprocess: %s\n", preprocess(str_in).c_str());

    }


    return 0;
}

