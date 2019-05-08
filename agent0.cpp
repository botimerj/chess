#include <string>
#include <fstream>

#include <cstdlib>
#include <unistd.h>     // getpid()
#include <sys/types.h>
#include <time.h>

// Custom libraries
#include <game.h>

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
    write_log_file(getpid(), "Start agent0\n");

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
            status += std::string("\n");
            write_log_file(getpid(), status.c_str());
        }
        if( std::string(in).compare("suggest") == 0){

            std::list<glm::ivec2> move_list;
            std::vector<glm::ivec2> piece_list;

            // Create a list of pieces
            for(int i = 0; i < 8; i++){
                for(int j = 0; j < 8; j++){
                    if(game->ts_color(game->bstate[i][j]) == game->turn){
                        piece_list.push_back(glm::ivec2(i,j));
                    }
                }
            }
            
            int idx;
            glm::ivec2 from;
            glm::ivec2 to;
            char move_str[5] = {'-','\0'};

            // Pick a random move for a piece (unless piece cannot move)
            while(move_list.empty()){
                if(piece_list.empty()){
                    break;
                }
                idx = rand() % piece_list.size();
                from = piece_list[idx];
                piece_list.erase(piece_list.begin() + idx);

                move_list = game->valid_moves(game->bstate[from.x][from.y], from);
            }

            if(game->result != -1){ // Game is over
                write(STDOUT_FILENO, move_str, strlen(move_str));
            }else if(!move_list.empty()){ // Found a valid move
                std::list<glm::ivec2>::iterator it = move_list.begin();
                std::advance(it, rand()%move_list.size());
                to = *it;

                move_str[0] = from.x+'a';
                move_str[1] = (8-from.y)+'0';
                move_str[2] = to.x+'a';
                move_str[3] = (8-to.y)+'0';
                move_str[4] = '\0';

                //printf("from(%d,%d) to(%d,%d)\n", from.x, from.y, to.x, to.y);
                //printf("%s\n", move_str);
                write(STDOUT_FILENO, move_str, strlen(move_str));
            }else{ // Did not find a valid move
                write(STDOUT_FILENO, move_str, strlen(move_str));
                //printf("No valid moves\n");
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

    write_log_file(getpid(), "Stop agent0\n");
}

