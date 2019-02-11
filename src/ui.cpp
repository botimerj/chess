#include <ui.h>

////////////////////////////////////
/// User Interface (UI) Functions //
////////////////////////////////////

UI::UI(GLFWwindow* window, int WIDTH, int HEIGHT){
    this->window = window;
    this->rm = new Resource_manager();
    this->resize_window(WIDTH, HEIGHT);


    // Render the board
    board = new Board(rm);
}

UI::~UI(){
    delete board;
    delete rm;
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
}

void UI::render(){

    get_mouse_pos();
    board->render(mouse_pos, aspect_ratio);
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

