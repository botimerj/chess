#include <string>
#include <fstream>

#include <cstdlib>
#include <unistd.h>     // getpid()
#include <sys/types.h>
#include <time.h>

// Custom libraries
#include <game.h>

int piece_value(TS piece){
    if(piece == TS::q || piece == TS::Q) return 10;
    if(piece == TS::r || piece == TS::R) return 5;
    if(piece == TS::b || piece == TS::B) return 3;
    if(piece == TS::n || piece == TS::N) return 3;
    if(piece == TS::p || piece == TS::P) return 1;
    if(piece == TS::k || piece == TS::K) return 1;
}

bool valid_move_string(char * in){
    std::string in_str(in);

    if(in_str.find_first_of("abcdefgh",0) != 0) return false;
    if(in_str.find_first_of("12345678",1) != 1) return false;
    if(in_str.find_first_of("abcdefgh",2) != 2) return false;
    if(in_str.find_first_of("12345678",3) != 3) return false;
    return true;
}

void write_log_file(int pid, const char * status){
    std::ofstream f_log;
    std::string pid_str = std::to_string(pid) + std::string(": ");
    std::string line = pid_str + std::string(status);
    f_log.open("log", std::ofstream::app);
    if(f_log.is_open()){
        f_log.write(line.c_str(), line.length());
        f_log.close();
    }else{
        printf("Error opening log file\n");
        exit(1);
    }
}

int main(){
    write_log_file(getpid(), "Start agent1\n");

    srand(time(NULL));
    Game gamestate;
    Game * game = &gamestate;
    char in[32];
    int read_len;

    while(1){
        //std::cin.getline(in, 256);
        read_len = read(STDIN_FILENO, in, 32);
        in[read_len - 1] = '\0';
        if( std::string(in).compare("kill") == 0 ){
            break;
        }
        if( valid_move_string(in) ){
            int x_to = in[2]-'a';
            int y_to = 7-(in[3]-'1');

            int x_from = in[0]-'a';
            int y_from = 7-(in[1]-'1');

            //printf("from(%d,%d) to(%d,%d): ", x_from, y_from, x_to, y_to);

            glm::ivec2 to(x_to,y_to);
            glm::ivec2 from(x_from,y_from);
            TS sel = game->bstate[x_from][y_from];

            std::string status;
            if(game->move(sel, to, from)){
                //printf("valid move\n");
                status = std::string("valid  |")+std::string(in);
            }else{
                //printf("invalid move\n");
                status = std::string("invalid|")+std::string(in);
            }
            //status += std::string("\n");
            write_log_file(getpid(), status.c_str());
        }
        if( std::string(in).compare("suggest") == 0){

            std::list<glm::ivec2> move_list;
            std::vector<glm::ivec2> piece_list;
            TS sel;

            // Create a list of pieces
            int max_total = -10000000;
            glm::ivec2 from = glm::ivec2(-1,-1);
            glm::ivec2 to = glm::ivec2(-1,-1);

            for(int i = 0; i < 8; i++){ for(int j = 0; j < 8; j++){
                if(game->ts_color(game->bstate[i][j]) == game->turn){
                    move_list = game->valid_moves(game->bstate[i][j], glm::ivec2(i,j));
                    for(std::list<glm::ivec2>::iterator itt = move_list.begin(); itt != move_list.end(); ++itt){
                        // Save old state
                        sel = game->bstate[i][j];
                        glm::ivec2 idx = *itt;
                        TS bstate_old = game->bstate[idx.x][idx.y];

                        game->bstate[i][j] = TS::e;
                        game->bstate[idx.x][idx.y] = sel;

                        // evaluate
                        int good_total = 0;
                        int bad_total = 0;
                        for(int k = 0; k < 8; k++){ for(int l = 0; l < 8; l++){
                            int mod = piece_value(game->bstate[k][l]);
                            if(game->ts_color(game->bstate[k][l]) == game->turn) good_total += 1;
                            else if( game->ts_color(game->bstate[k][l]) == !game->turn ) bad_total  += 1;
                        }}
                        int total = good_total - bad_total;
                        if(total-max_total>0 || (rand()%10 > 8)){
                            from = glm::ivec2(i,j);
                            to = idx;
                            max_total = total;
                        }

                        // Reset to old state
                        game->bstate[i][j] = sel;
                        game->bstate[idx.x][idx.y] = bstate_old;
                    }
                }
            }}
            
            char move_str[5] = {'-','\0'};
            if(to.x == -1 || game->result != -1){
                //no valid moves or game is over
                write(STDOUT_FILENO, move_str, strlen(move_str));
            }else{
                move_str[0] = from.x+'a';
                move_str[1] = (8-from.y)+'0';
                move_str[2] = to.x+'a';
                move_str[3] = (8-to.y)+'0';
                move_str[4] = '\0';

                write(STDOUT_FILENO, move_str, strlen(move_str));
            }
            std::string status;
            status = std::string("suggest|")+std::string(move_str)+std::string("\n");
            write_log_file(getpid(), status.c_str());
        }
        if( std::string(in).compare("reset") == 0){
            game->set_board(game->default_pos);
            write_log_file(getpid(), "reset\n");
        }
    }
    write_log_file(getpid(), "Stop agent1\n");
}

