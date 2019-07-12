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

    // Agent threads
    kill_agent_w = true;
    kill_agent_b = true;
    clear_log_file();

    // Render variables
    render_continuously = false;
    render_flag = true;

}

UI::~UI(){
    // Delete resources 
    delete board;
    delete tbox;
    delete rm;
    delete game;

    // End threads and join 
    if(!kill_agent_w){
        kill_agent_w = true;
        t_agent_w.join();
    }
    if(!kill_agent_b){
        kill_agent_b = true;
        t_agent_b.join();
    }

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

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    get_mouse_pos();
    //menu->render(aspect_ratio);
    board->render(mouse_pos, aspect_ratio);
    //tbox->render();
    
    glfwSwapBuffers(window);
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
    if(down) render_continuously = true;
    else render_continuously = false;

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
            board->reset();
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
        if( std::string(in).compare("computer_w") == 0 ) {
            if(kill_agent_w == true){
                kill_agent_w = false;
                t_agent_w = std::thread(&UI::agent_handler, this, 0, "./agent2"); 
            }else{
                printf("Please kill the agent_w before starting again\n");
            }
        }
        if( !kill_agent_w && std::string(in).compare("kill_w") == 0 ) {
            kill_agent_w = true;
            t_agent_w.join();
        }
        if( std::string(in).compare("computer_b") == 0 ) {
            if(kill_agent_b == true){
                kill_agent_b = false;
                t_agent_b = std::thread(&UI::agent_handler, this, 1, "./agent2"); 
            }else{
                printf("Please kill the agent_b before starting again\n");
            }
        }
        if( !kill_agent_b && std::string(in).compare("kill_b") == 0 ) {
            kill_agent_b = true;
            t_agent_b.join();
        }
        render_flag = true;
    }
}

// Agent funcitons
void UI::agent_handler(int color, const char * agent){

    //clear_log_file();
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

        write_log_file(getpid(), "Starting agent (from child)\n");
        execl(agent, agent, (char *)0);
        printf("Error!");

    }else{
        write_log_file(getpid(), "Begin parent\n");

        close(i_pipe[PIPE_READ]);
        close(o_pipe[PIPE_WRITE]);

        char readbuf[32];
        const char * suggest_str = "suggest\n";
        const char * kill_str = "kill\n";
        ssize_t readsize;
        std::string log_string;

        bool *kill_agent;
        if(color == 0) kill_agent = &kill_agent_w;
        else           kill_agent = &kill_agent_b;
        while(!(*kill_agent)){

            
            if(color != game->turn){
                usleep(200000);
                continue;
            }

            if(game->last_move[0] != '-'){
                std::string write_move = game->last_move + std::string("\n");
                write(i_pipe[PIPE_WRITE], write_move.c_str(), strlen(suggest_str));
                log_string = std::string("wrote last move: ") + game->last_move + std::string("\n");
                write_log_file(getpid(), log_string.c_str());
            }

            write(i_pipe[PIPE_WRITE], suggest_str, strlen(suggest_str));
            write_log_file(getpid(), "asked for suggestion\n");
            usleep(500000);

            readsize = read(o_pipe[PIPE_READ], readbuf, 32);
            if(readsize > 0){
                if(readbuf[0] == '-'){
                    std::cout << "Killing agent" << std::endl;
                    //kill_agent_w = true;
                    break;
                }

                log_string = std::string("got suggestion: ") + std::string(readbuf) + std::string("\n");
                write_log_file(getpid(), std::string(log_string).c_str());
                 
                int x_to = readbuf[2]-'a';
                int y_to = 7-(readbuf[3]-'1');

                int x_from = readbuf[0]-'a';
                int y_from = 7-(readbuf[1]-'1');

                glm::ivec2 to(x_to,y_to);
                glm::ivec2 from(x_from,y_from);
                TS sel = game->bstate[x_from][y_from];
                if(game->move(sel, to, from)){
                    board->move(from, to);
                    render_flag = true;
                }

                readbuf[readsize] = '\n';
                write(i_pipe[PIPE_WRITE], readbuf, strlen(suggest_str));

                log_string = std::string("wrote move: ") + std::string(readbuf);
                write_log_file(getpid(), std::string(log_string).c_str());
            }
        }

        write(i_pipe[PIPE_WRITE], kill_str, strlen(kill_str));

        close(o_pipe[PIPE_READ]);
        write_log_file(getpid(), "End (from parent)\n");
    }

    write_log_file(getpid(), "End agent handler\n");
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

// Helper function
bool UI::valid_move_string(char * in){
    std::string in_str(in);

    if(in_str.find_first_of("abcdefgh",0) != 0) return false;
    if(in_str.find_first_of("12345678",1) != 1) return false;
    if(in_str.find_first_of("abcdefgh",2) != 2) return false;
    if(in_str.find_first_of("12345678",3) != 3) return false;
    return true;

}

