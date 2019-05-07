#include <ui.h>

////////////////////////////////////
/// User Interface (UI) Functions //
////////////////////////////////////

UI::UI(GLFWwindow* window, int WIDTH, int HEIGHT){
    this->window = window;
    this->rm = new Resource_manager(WIDTH);
    this->resize_window(WIDTH, HEIGHT);

    // Create game state
    game = new Game();

    // Render the board
    board = new Board(rm, game);
    tbox = new Tbox(rm->text);

    // Render Menu
    menu = new Menu(rm);

    // Start listening for std in 
    exit = true; // I'm pretty sure this could case a race
    t_listen = std::thread(&UI::listen, this); 

    kill_agent = true;
}

UI::~UI(){
    // Delete resources 
    delete board;
    delete tbox;
    delete rm;
    delete game;

    // End threads and join 
    kill_agent = true;
    t_agent.join();

    exit = false;
    t_listen.join();
}

void UI::resize_window(int width, int height){
    this->aspect_ratio = static_cast<float>(width)/static_cast<float>(height);
    this->WIDTH = width;
    this->HEIGHT = height;
    this->projection = glm::ortho( -aspect_ratio, aspect_ratio, 
                                   1.0f, -1.0f, 
                                  -1.0f, 1.0f);
    this->rm->shader[0].use();
    this->rm->shader[0].SetMatrix4f("projection", projection);
    this->rm->shader[0].SetMatrix4f("view", glm::mat4(1.0f));

    this->rm->shader[1].use();
    this->rm->shader[1].SetMatrix4f("projection", projection);
    this->rm->shader[1].SetMatrix4f("view", glm::mat4(1.0f));
}

void UI::render(){

    get_mouse_pos();
    //menu->render(aspect_ratio);
    board->render(mouse_pos, aspect_ratio);
    //tbox->render();
}

void UI::right_click(int action){
    get_mouse_pos();
    if(action == GLFW_PRESS){
        glm::vec2 board_mouse_pos(aspect_ratio + mouse_pos.x - board->coor.x,
                                  1.0f + mouse_pos.y - board->coor.y);
        board->board_right_click(board_mouse_pos);
    }
}

void UI::left_click(int action){
    bool down = (action == GLFW_PRESS);
    get_mouse_pos();

    // Board left click
    glm::vec2 board_mouse_pos(aspect_ratio + mouse_pos.x - board->coor.x,
                              1.0f + mouse_pos.y - board->coor.y);
    board->board_left_click(board_mouse_pos, down);

}

void UI::get_mouse_pos(){
    double xposd, yposd;
    glfwGetCursorPos(window, &xposd, &yposd);
    mouse_pos.x = static_cast<float>(2.0f*aspect_ratio/WIDTH*xposd - aspect_ratio);
    mouse_pos.y = static_cast<float>(2.0f/HEIGHT*yposd - 1);
}

void UI::listen(){
    char in[256];
    while(exit){
        std::cin.getline(in, 256);
        if( std::string(in).compare("reset") == 0 ){
            game->set_board(game->default_pos);
        }
        if( std::string(in).compare("print") == 0 ) {
            std::cout << game->gs_to_fen() << std::endl;
        }
        if( std::string(in).compare("set") == 0 ) {
            // open file, read fen string, and set board
            std::fstream f;
            f.open("fen");
            if(!f.is_open()){
                std::cout << "Error opening file" << std::endl;
            }
            char line[256];
            f.getline(line,256);
            game->set_board(std::string(line));
            f.close();
        }
        if( std::string(in).compare("flip") == 0 ) {
            board->flip = !board->flip;
        }
        if( valid_move_string(in) ){

            int x_to = in[2]-'a';
            int y_to = 7-(in[3]-'1');

            int x_from = in[0]-'a';
            int y_from = 7-(in[1]-'1');
            //printf("Valid move; from(%d,%d) to(%d,%d)\n", x_from, y_from, x_to, y_to);

            glm::ivec2 to(x_to,y_to);
            glm::ivec2 from(x_from,y_from);
            TS sel = game->bstate[x_from][y_from];

            if(game->move(sel, to, from)){
                board->move(from, to);
            }
        }
        if( std::string(in).compare("computer") == 0 ) {
            // Spawn agent 0
            kill_agent = false;
            t_agent = std::thread(&UI::agent_handler, this); 
        }
        if( std::string(in).compare("kill") == 0 ) {
            kill_agent = true;
            t_agent.join();
        }
    }
}

void UI::agent_handler(){

    clear_log_file();
    write_log_file(getpid(), "Start agent handler\n");

    int i_pipe[2];
    int o_pipe[2];

    if(pipe(i_pipe) < 0) printf("Error on pipe()");
    if(pipe(o_pipe) < 0) printf("Error on pipe()"); 

    pid_t pid = fork();

    if(pid < 0){
        close(i_pipe[PIPE_READ]); close(i_pipe[PIPE_WRITE]);
        close(o_pipe[PIPE_READ]); close(o_pipe[PIPE_WRITE]);
        printf("Error on fork()");
    }else if(pid == 0){

        if (dup2(i_pipe[PIPE_READ], STDIN_FILENO) == -1) printf("Error on dup2()");
        if (dup2(o_pipe[PIPE_WRITE], STDOUT_FILENO) == -1) printf("Error on dup2()");

        close(i_pipe[PIPE_READ]); close(i_pipe[PIPE_WRITE]);
        close(o_pipe[PIPE_READ]); close(o_pipe[PIPE_WRITE]);

        write_log_file(getpid(), "Starting agent0 (from child)\n");
        execl("./agent0", "./agent0", (char *)0);
        printf("Error!");

    }else{
        write_log_file(getpid(), "Begin parent\n");

        close(i_pipe[PIPE_READ]);
        close(o_pipe[PIPE_WRITE]);

        char readbuf[32];
        const char * suggest_str = "suggest\n";
        ssize_t readsize;
        std::string log_string;

        while(!kill_agent){
            
            write(i_pipe[PIPE_WRITE], suggest_str, strlen(suggest_str));
            write_log_file(getpid(), "asked for suggestion\n");

            readsize = read(o_pipe[PIPE_READ], readbuf, 32);
            if(readsize > 0){
                log_string = std::string("got suggestion: ") + std::string(readbuf);
                write_log_file(getpid(), std::string(log_string).c_str());

                //readbuf[readsize] = '\0';

                int x_to = readbuf[2]-'a';
                int y_to = 7-(readbuf[3]-'1');

                int x_from = readbuf[0]-'a';
                int y_from = 7-(readbuf[1]-'1');
                //printf("Valid move; from(%d,%d) to(%d,%d)\n", x_from, y_from, x_to, y_to);

                glm::ivec2 to(x_to,y_to);
                glm::ivec2 from(x_from,y_from);
                TS sel = game->bstate[x_from][y_from];
                if(game->move(sel, to, from)){
                    board->move(from, to);
                }

                readbuf[readsize] = '\n';
                write(i_pipe[PIPE_WRITE], readbuf, strlen(suggest_str));

                log_string = std::string("wrote move: ") + std::string(readbuf);
                write_log_file(getpid(), std::string(log_string).c_str());

            }
            usleep(2000000);
        }

        const char * kill_str = "kill\n";
        write(i_pipe[PIPE_WRITE], kill_str, strlen(kill_str));

        close(o_pipe[PIPE_READ]);
        write_log_file(getpid(), "End (from parent)\n");
    }

    write_log_file(getpid(), "End agent handler\n");
}

// Helper function
bool UI::valid_move_string(char * in){
    std::string in_str(in);

    if(in_str.find_first_of("abcdefgh",0) != 0) return false;
    if(in_str.find_first_of("12345678",1) != 1) return false;
    if(in_str.find_first_of("abcdefgh",2) != 2) return false;
    if(in_str.find_first_of("12345678",3) != 3) return false;
    return true;

}

void UI::write_log_file(int pid, const char * status){
    std::ofstream f_log;
    std::string pid_str = std::to_string(pid) + std::string(": ");
    std::string line = pid_str + std::string(status);
    f_log.open("log", std::ofstream::app);
    if(f_log.is_open()){
        f_log.write(line.c_str(), line.length());
        f_log.close();
    }else{
        printf("Error opening log file\n");
    }
}

void UI::clear_log_file(){
    std::ofstream f_log;
    f_log.open("log");
    if(f_log.is_open()){
        f_log.close();
    }else{
        printf("Error opening log file\n");
    }
}


