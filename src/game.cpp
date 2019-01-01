#include <game.h>


GameState::GameState(GLFWwindow* window, int WIDTH, int HEIGHT){
    this->window = window;
    // Load Shader
    shaders = new Shader[1];
    this->shaders[0].init("shader/shader.vs", "shader/shader.fs");

    // Load Texture
    textures = new Texture2D[2];
    int width, height, nrChannels;
    unsigned char *data;
    data = stbi_load("textures/square.png", &width, &height, &nrChannels, 0);
    this->textures[0].init(data, width, height, true);
    stbi_image_free(data);
    data = stbi_load("textures/ChessPiecesArray.png", &width, &height, &nrChannels, 0);
    this->textures[1].init(data, width, height, true);
    stbi_image_free(data);

    this->resizeWindow(WIDTH, HEIGHT);

    //Create Pieces Array
    float x_offset, y_offset;
    for(int y = 0; y < 2; y++){
        for(int x = 0; x < 6; x++){
            pieces.push_back(Piece(x, y));
        } 
    }

    // Tile Color arr
    this->color_arr[0] = glm::vec3(0.2f,0.2f,0.2f); // black
    this->color_arr[1] = glm::vec3(0.8f,0.8f,0.8f); // white 
   
    // Game state
    b_kside_castle = true;
    b_qside_castle = true;
    w_kside_castle = true;
    w_qside_castle = true;
    turn = Color::white;
    played_from_idx = glm::vec2(-1.0f,-1.0f);
    played_idx = glm::vec2(-1.0f,-1.0f);
    for(int y = 0; y < 8; y++){
        for(int x = 0; x < 8; x++){
            this->bstate[x][y] = Board::empty;
        }
    }
    this->bstate[0][0] = Board::b_rook;
    this->bstate[1][0] = Board::b_knight;
    this->bstate[2][0] = Board::b_bishop;
    this->bstate[3][0] = Board::b_queen;
    this->bstate[4][0] = Board::b_king;
    this->bstate[5][0] = Board::b_bishop;
    this->bstate[6][0] = Board::b_knight;
    this->bstate[7][0] = Board::b_rook;

    this->bstate[0][7] = Board::w_rook;
    this->bstate[1][7] = Board::w_knight;
    this->bstate[2][7] = Board::w_bishop;
    this->bstate[3][7] = Board::w_queen;
    this->bstate[4][7] = Board::w_king;
    this->bstate[5][7] = Board::w_bishop;
    this->bstate[6][7] = Board::w_knight;
    this->bstate[7][7] = Board::w_rook;

    for(int i = 0; i < 8; i++){
        this->bstate[i][1] = Board::b_pawn;
        this->bstate[i][6] = Board::w_pawn;

    }

    // Mouse 
    this->mouse_pos = glm::vec2(0.0f,0.0f);
    this->selected = Board::empty;
    this->selected_idx = glm::vec2(-1.0f,-1.0f);
    this->check_idx = glm::vec2(-1.0f,-1.0f);

    // Visuals
    this->border = glm::vec2(0.15f,0.15f);
    this->tile_size = (2.0f-border.x*2)/8.0f;

}

GameState::~GameState(){
    delete[] shaders;
    delete[] textures;
}

void GameState::render(){
    this->shaders[0].use();
    this->shaders[0].SetMatrix4f("view", glm::mat4(1.0f));

    // Draw board and pieces
    glm::vec2 pos(-this->aspect_ratio+this->border.x+this->tile_size/2,
                  -1.0f+this->border.y+this->tile_size/2);
    int t = 1;
    for(int y = 0; y < 8; y++){
        for(int x = 0; x < 8; x++){
            // Paint Tiles, shade if they are selected
            if(check_idx.x == x && check_idx.y == y){
                glm::vec3 color = color_arr[t]*glm::vec3(1.0f,0.2f,0.2f);
                this->tile.draw(this->shaders[0],this->textures[0], tile_size, color, pos);
            } else if(x == selected_idx.x && y == selected_idx.y){
                glm::vec3 color = color_arr[t]*glm::vec3(0.4f,0.4f,1.0f);
                this->tile.draw(this->shaders[0],this->textures[0], tile_size, color, pos);
            } else if(x == played_from_idx.x && y == played_from_idx.y ||
                      x == played_idx.x && y == played_idx.y ){
                glm::vec3 color = color_arr[t]*glm::vec3(0.6f,0.6f,1.0f);
                this->tile.draw(this->shaders[0],this->textures[0], tile_size, color, pos);
            } else{
                    this->tile.draw(this->shaders[0],this->textures[0], tile_size, color_arr[t], pos);
            }
            // Paint Pieces
            int piece_idx = static_cast<int> (this->bstate[x][y]);
            if(piece_idx != Board::empty){
                this->pieces[piece_idx].draw(this->shaders[0], this->textures[1], this->tile_size, pos);
            }
            t = (t+1)%2;
            pos = glm::vec2(pos.x+this->tile_size, pos.y);
        }
        pos = glm::vec2(-this->aspect_ratio+this->border.x+this->tile_size/2, pos.y+this->tile_size);
        t = (t+1)%2;
    }

    // Draw Selected Piece
    if(this->selected != Board::empty){
        this->getMousePos();
        pieces[selected].draw(this->shaders[0], this->textures[1], this->tile_size, this->mouse_pos);
    }



}

void GameState::processInput(){
    if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(this->window, true);
}

void GameState::resizeWindow(int width, int height){
    this->aspect_ratio = static_cast<float>(width)/static_cast<float>(height);
    this->WIDTH = width;
    this->HEIGHT = height;
    this->projection = glm::ortho( -this->aspect_ratio, this->aspect_ratio, 
                                   1.0f, -1.0f, 
                                  -1.0f, 1.0f);
    this->shaders[0].use();
    this->shaders[0].SetMatrix4f("projection", projection);
}

void GameState::leftClick(int action){
    if(action == GLFW_PRESS){
        this->getMousePos();
        int xidx, yidx;
        this->getBoardIdx(&xidx, &yidx);
        this->selected_idx = glm::vec2(xidx, yidx);
        if(xidx != -1 && yidx != -1){
            this->selected = bstate[xidx][yidx];
            bstate[xidx][yidx] = empty;
        }
    }
    else if(action == GLFW_RELEASE){
        this->getMousePos();
        int xidx, yidx;
        this->getBoardIdx(&xidx, &yidx);
        if(this->selected != Board::empty){ 
            if(xidx == -1 || yidx == -1){
                bstate[int(this->selected_idx.x)][int(this->selected_idx.y)] = this->selected;
            }
            else{
                this->rules(glm::vec2(xidx,yidx));
                //bstate[xidx][yidx] = this->selected;
                //this->turn = static_cast<Turn>((static_cast<int>(this->turn)+1)%2);
            }
            this->selected = Board::empty;
        }
        this->selected_idx = glm::vec2(-1.0f,-1.0f);
        // Testing
        //selected_attackers.erase(selected_attackers.begin(), selected_attackers.end());
    }
}

void GameState::getMousePos(){
    double xposd, yposd;
    glfwGetCursorPos(this->window, &xposd, &yposd);
    this->mouse_pos.x = static_cast<float>(2.0f*this->aspect_ratio/this->WIDTH*xposd - this->aspect_ratio);
    this->mouse_pos.y = static_cast<float>(2.0f/this->HEIGHT*yposd - 1);
}

void GameState::getBoardIdx(int *xidx, int *yidx){
    float xposd = this->mouse_pos.x;
    float yposd = this->mouse_pos.y;

    // Check if a piece was selected 
    xposd = xposd - this->border.x + this->aspect_ratio;
    yposd = yposd - this->border.y + 1;

    xposd = floor(xposd/this->tile_size);
    yposd = floor(yposd/this->tile_size);

    if(xposd > 7 || xposd < 0)
        xposd = -1;
    if(yposd > 7 || yposd < 0)
        yposd = -1;

    *xidx = static_cast<int>(xposd);
    *yidx = static_cast<int>(yposd);
}

void GameState::rules(glm::vec2 new_idx){

    bool en_passant = false;
    bool castle = false; 
    bool promote = false;

    // Rule: Players alternate turns
    if(this->turn != this->piece_color(selected)){
        bstate[int(this->selected_idx.x)][int(this->selected_idx.y)] = this->selected;
        return;
    }
    // Rule: Has to be a valid move
    if(!movement(new_idx, &en_passant, &castle, &promote)){
        bstate[int(this->selected_idx.x)][int(this->selected_idx.y)] = this->selected;
        return;
    }
    
    // Save old state
    Board old_piece = bstate[int(new_idx.x)][int(new_idx.y)];
    bstate[int(new_idx.x)][int(new_idx.y)] = this->selected;

    // Remove en passant pawn
    //printf("en passant: %d | promote: %d | castle: %d\n",en_passant, promote, castle);
    if(en_passant){
        if(turn == Color::black){
            bstate[int(new_idx.x)][int(new_idx.y-1)] = Board::empty;
        }
        if(turn == Color::white){
            bstate[int(new_idx.x)][int(new_idx.y+1)] = Board::empty;
        }
    }

    // Promote (awlays promote queen for now)
    if(promote){
        if(turn == Color::white)
            bstate[int(new_idx.x)][int(new_idx.y)] = Board::w_queen;
        if(turn == Color::black)
            bstate[int(new_idx.x)][int(new_idx.y)] = Board::b_queen;
    }

    // Check if king is in check
    // Find king idx
    glm::vec2 b_k_idx;
    glm::vec2 w_k_idx;
    for(int y = 0; y < 8; y++){
        for(int x = 0; x < 8; x++){
            if(bstate[x][y] == Board::b_king)
                b_k_idx = glm::vec2(x,y);
            if(bstate[x][y] == Board::w_king)
                w_k_idx = glm::vec2(x,y);
        }
    }
    if(turn == Color::black){
        // See if you moved into check
        if(tile_attackers(b_k_idx, Color::white).empty() == false){
            // Revert
            bstate[int(this->selected_idx.x)][int(this->selected_idx.y)] = this->selected;
            bstate[int(new_idx.x)][int(new_idx.y)] = old_piece;
            if(en_passant){
                bstate[int(new_idx.x)][int(new_idx.y-1)] = Board::w_pawn;
            }
            return;
        }
        // See if you put opp in check
        if(tile_attackers(w_k_idx, Color::black).empty() == false)
            check_idx = w_k_idx;
        else 
            check_idx = glm::vec2(-1,-1);
    }
    if(turn == Color::white){
        // See if you moved into check
        if(tile_attackers(w_k_idx, Color::black).empty() == false){
            // Revert
            bstate[int(this->selected_idx.x)][int(this->selected_idx.y)] = this->selected;
            bstate[int(new_idx.x)][int(new_idx.y)] = old_piece;
            if(en_passant){
                bstate[int(new_idx.x)][int(new_idx.y+1)] = Board::b_pawn;
            }
            return;
        }
        // See if you put opp in check
        if(tile_attackers(b_k_idx, Color::white).empty() == false)
            check_idx = b_k_idx;
        else 
            check_idx = glm::vec2(-1,-1);
    }
    
    // If we made it to here the move was valid
    // Castle 
    if(castle){
        if(new_idx == glm::vec2(6,7)){
            bstate[5][7] = Board::w_rook;
            bstate[7][7] = Board::empty;
        } else if(new_idx == glm::vec2(2,7)){
            bstate[3][7] = Board::w_rook;
            bstate[0][7] = Board::empty;
        } else if(new_idx == glm::vec2(6,0)){
            bstate[5][0] = Board::b_rook;
            bstate[7][0] = Board::empty;
        } else if(new_idx == glm::vec2(2,0)){
            bstate[3][0] = Board::b_rook;
            bstate[0][0] = Board::empty;
        }
    }

    // Check if rooks or kings were moved to remove castleing potential
    if(selected_idx == glm::vec2(0,0))
        b_qside_castle = false;
    if(selected_idx == glm::vec2(7,0))
        b_kside_castle = false;
    if(selected_idx == glm::vec2(0,7))
        w_qside_castle = false;
    if(selected_idx == glm::vec2(7,7))
        w_kside_castle = false;

    if(selected_idx == glm::vec2(4,7)){
        w_qside_castle = false;
        w_kside_castle = false;
    }
    if(selected_idx == glm::vec2(4,0)){
        b_qside_castle = false;
        b_kside_castle = false;
    }

    // Update turn 
    this->turn = static_cast<Color>((static_cast<int>(this->turn)+1)%2);
    played_from_idx = selected_idx;
    played_idx = new_idx;
}

Color GameState::piece_color(Board piece){
    if(piece >= 0 && piece <  6)
        return Color::black;
    else if(piece >= 6 && piece < 12)
        return Color::white;
    else
        return Color::none;
}

bool GameState::movement(glm::vec2 new_idx, bool *en_passant, bool *castle, bool *promote){
    std::vector<glm::vec2> valid_moves;
    int en_passant_idx = -1;
    *en_passant = false;
    *castle = false;

    ////////////////
    ///   Pawn   ///
    ////////////////

    // White pawn
    if(selected == Board::w_pawn){
        // Move forward 1 
        if(bstate[int(selected_idx.x)][int(selected_idx.y-1)] == Board::empty){
            valid_moves.push_back(glm::vec2(selected_idx.x,selected_idx.y-1));
            // Move forward 2 if still in original location
            if(selected_idx.y == 6){
                if(bstate[int(selected_idx.x)][int(selected_idx.y-2)] == Board::empty)
                    valid_moves.push_back(glm::vec2(selected_idx.x,selected_idx.y-2));
            }
        }
        // Take
        if(piece_color(bstate[int(selected_idx.x-1)][int(selected_idx.y-1)]) == Color::black)
            valid_moves.push_back(glm::vec2(selected_idx.x-1,selected_idx.y-1));
        if(piece_color(bstate[int(selected_idx.x+1)][int(selected_idx.y-1)]) == Color::black)
            valid_moves.push_back(glm::vec2(selected_idx.x+1,selected_idx.y-1));

        // En passante
        if((bstate[int(selected_idx.x+1)][int(selected_idx.y)]) == Board::b_pawn && played_from_idx.y == 1){
            valid_moves.push_back(glm::vec2(selected_idx.x+1,selected_idx.y-1));
            en_passant_idx = valid_moves.size();
        }
        if((bstate[int(selected_idx.x-1)][int(selected_idx.y)]) == Board::b_pawn && played_from_idx.y == 1){
            valid_moves.push_back(glm::vec2(selected_idx.x-1,selected_idx.y-1));
            en_passant_idx = valid_moves.size();
        }
    }

    // Black pawn
    if(selected == Board::b_pawn){
        // Move forward 1 
        if(bstate[int(selected_idx.x)][int(selected_idx.y+1)] == Board::empty){
            valid_moves.push_back(glm::vec2(selected_idx.x,selected_idx.y+1));
            // Move forward 2 if still in original location
            if(selected_idx.y == 1){
                if(bstate[int(selected_idx.x)][int(selected_idx.y+2)] == Board::empty)
                    valid_moves.push_back(glm::vec2(selected_idx.x,selected_idx.y+2));
            }
        }
        // Take
        if(piece_color(bstate[int(selected_idx.x-1)][int(selected_idx.y+1)]) == Color::white)
            valid_moves.push_back(glm::vec2(selected_idx.x-1,selected_idx.y+1));
        if(piece_color(bstate[int(selected_idx.x+1)][int(selected_idx.y+1)]) == Color::white)
            valid_moves.push_back(glm::vec2(selected_idx.x+1,selected_idx.y+1));

        // En passante
        if((bstate[int(selected_idx.x+1)][int(selected_idx.y)]) == Board::w_pawn && played_from_idx.y == 6){
            valid_moves.push_back(glm::vec2(selected_idx.x+1,selected_idx.y+1));
            en_passant_idx = valid_moves.size();
        }
        if((bstate[int(selected_idx.x-1)][int(selected_idx.y)]) == Board::w_pawn && played_from_idx.y == 6){
            valid_moves.push_back(glm::vec2(selected_idx.x-1,selected_idx.y+1));
            en_passant_idx = valid_moves.size();
        }
    }


    ////////////////
    ///  Knight  ///
    ////////////////

    if(selected == Board::w_knight || selected == Board::b_knight){
        glm::vec2 idx_check[8];
        idx_check[0] = glm::vec2(selected_idx.x-2,selected_idx.y-1);
        idx_check[1] = glm::vec2(selected_idx.x-2,selected_idx.y+1);
        idx_check[2] = glm::vec2(selected_idx.x+2,selected_idx.y-1);
        idx_check[3] = glm::vec2(selected_idx.x+2,selected_idx.y+1);
        idx_check[4] = glm::vec2(selected_idx.x-1,selected_idx.y-2);
        idx_check[5] = glm::vec2(selected_idx.x-1,selected_idx.y+2);
        idx_check[6] = glm::vec2(selected_idx.x+1,selected_idx.y-2);
        idx_check[7] = glm::vec2(selected_idx.x+1,selected_idx.y+2);
        for(int i = 0; i < 8; i++){
            // check if its in bounds, checks if it your own piece
            if(idx_check[i].x < 8 && idx_check[i].x >= 0 && 
               idx_check[i].y < 8 && idx_check[i].y >= 0 && 
               piece_color(bstate[int(idx_check[i].x)][int(idx_check[i].y)]) != piece_color(selected)){
                valid_moves.push_back(glm::vec2(idx_check[i].x, idx_check[i].y));
            }
        }
    }

    ////////////////
    ///   Rook   ///
    ////////////////

    if(selected == Board::w_rook || selected == Board::b_rook){
        glm::vec2 idx_check(selected_idx.x-1, selected_idx.y);
        bool intersect = false;
        // Look left
        while(  piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != piece_color(selected) &&
                intersect == false &&
                idx_check.x >= 0){
            if(piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != Color::none)
                intersect = true;
            valid_moves.push_back(glm::vec2(idx_check.x, idx_check.y));
            idx_check = glm::vec2(idx_check.x-1, idx_check.y);
        }
        // Look right 
        idx_check = glm::vec2(selected_idx.x+1, selected_idx.y);
        intersect = false;
        while(  piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != piece_color(selected) &&
                intersect == false &&
                idx_check.x <= 7){
            if(piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != Color::none)
                intersect = true;
            valid_moves.push_back(glm::vec2(idx_check.x, idx_check.y));
            idx_check = glm::vec2(idx_check.x+1, idx_check.y);
        }
        // Look up 
        idx_check = glm::vec2(selected_idx.x, selected_idx.y-1);
        intersect = false;
        while(  piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != piece_color(selected) &&
                intersect == false &&
                idx_check.y >= 0){
            if(piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != Color::none)
                intersect = true;
            valid_moves.push_back(glm::vec2(idx_check.x, idx_check.y));
            idx_check = glm::vec2(idx_check.x, idx_check.y-1);
        }

        // Look down 
        idx_check = glm::vec2(selected_idx.x, selected_idx.y+1);
        intersect = false;
        while(  piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != piece_color(selected) &&
                intersect == false &&
                idx_check.y <= 7){
            if(piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != Color::none)
                intersect = true;
            valid_moves.push_back(glm::vec2(idx_check.x, idx_check.y));
            idx_check = glm::vec2(idx_check.x, idx_check.y+1);
        }
    }

    ////////////////
    ///  Bishop  ///
    ////////////////

    if(selected == Board::w_bishop || selected == Board::b_bishop){
        glm::vec2 idx_check(selected_idx.x-1, selected_idx.y-1);
        bool intersect = false;
        // Look left,up
        while(  piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != piece_color(selected) &&
                intersect == false &&
                idx_check.x >= 0 && idx_check.y >= 0){
            if(piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != Color::none)
                intersect = true;
            valid_moves.push_back(glm::vec2(idx_check.x, idx_check.y));
            idx_check = glm::vec2(idx_check.x-1, idx_check.y-1);
        }
        // Look right,up 
        idx_check = glm::vec2(selected_idx.x+1, selected_idx.y-1);
        intersect = false;
        while(  piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != piece_color(selected) &&
                intersect == false &&
                idx_check.x <= 7 && idx_check.y >= 0){
            if(piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != Color::none)
                intersect = true;
            valid_moves.push_back(glm::vec2(idx_check.x, idx_check.y));
            idx_check = glm::vec2(idx_check.x+1, idx_check.y-1);
        }
        // Look left,down 
        idx_check = glm::vec2(selected_idx.x-1, selected_idx.y+1);
        intersect = false;
        while(  piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != piece_color(selected) &&
                intersect == false &&
                idx_check.x >= 0 && idx_check.y <= 7){
            if(piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != Color::none)
                intersect = true;
            valid_moves.push_back(glm::vec2(idx_check.x, idx_check.y));
            idx_check = glm::vec2(idx_check.x-1, idx_check.y+1);
        }

        // Look right,down 
        idx_check = glm::vec2(selected_idx.x+1, selected_idx.y+1);
        intersect = false;
        while(  piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != piece_color(selected) &&
                intersect == false &&
                idx_check.x <= 7 && idx_check.y <= 7){
            if(piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != Color::none)
                intersect = true;
            valid_moves.push_back(glm::vec2(idx_check.x, idx_check.y));
            idx_check = glm::vec2(idx_check.x+1, idx_check.y+1);
        }
    }

    ////////////////
    ///  Queen   ///
    ////////////////

    if(selected == Board::w_queen || selected == Board::b_queen){
        glm::vec2 idx_check(selected_idx.x-1, selected_idx.y);
        bool intersect = false;
        // Look left
        while(  piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != piece_color(selected) &&
                intersect == false &&
                idx_check.x >= 0){
            if(piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != Color::none)
                intersect = true;
            valid_moves.push_back(glm::vec2(idx_check.x, idx_check.y));
            idx_check = glm::vec2(idx_check.x-1, idx_check.y);
        }
        // Look right 
        idx_check = glm::vec2(selected_idx.x+1, selected_idx.y);
        intersect = false;
        while(  piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != piece_color(selected) &&
                intersect == false &&
                idx_check.x <= 7){
            if(piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != Color::none)
                intersect = true;
            valid_moves.push_back(glm::vec2(idx_check.x, idx_check.y));
            idx_check = glm::vec2(idx_check.x+1, idx_check.y);
        }
        // Look up 
        idx_check = glm::vec2(selected_idx.x, selected_idx.y-1);
        intersect = false;
        while(  piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != piece_color(selected) &&
                intersect == false &&
                idx_check.y >= 0){
            if(piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != Color::none)
                intersect = true;
            valid_moves.push_back(glm::vec2(idx_check.x, idx_check.y));
            idx_check = glm::vec2(idx_check.x, idx_check.y-1);
        }

        // Look down 
        idx_check = glm::vec2(selected_idx.x, selected_idx.y+1);
        intersect = false;
        while(  piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != piece_color(selected) &&
                intersect == false &&
                idx_check.y <= 7){
            if(piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != Color::none)
                intersect = true;
            valid_moves.push_back(glm::vec2(idx_check.x, idx_check.y));
            idx_check = glm::vec2(idx_check.x, idx_check.y+1);
        }
    
        // Look left,up
        idx_check = glm::vec2(selected_idx.x-1, selected_idx.y-1);
        intersect = false;
        while(  piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != piece_color(selected) &&
                intersect == false &&
                idx_check.x >= 0 && idx_check.y >= 0){
            if(piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != Color::none)
                intersect = true;
            valid_moves.push_back(glm::vec2(idx_check.x, idx_check.y));
            idx_check = glm::vec2(idx_check.x-1, idx_check.y-1);
        }
        // Look right,up 
        idx_check = glm::vec2(selected_idx.x+1, selected_idx.y-1);
        intersect = false;
        while(  piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != piece_color(selected) &&
                intersect == false &&
                idx_check.x <= 7 && idx_check.y >= 0){
            if(piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != Color::none)
                intersect = true;
            valid_moves.push_back(glm::vec2(idx_check.x, idx_check.y));
            idx_check = glm::vec2(idx_check.x+1, idx_check.y-1);
        }
        // Look left,down 
        idx_check = glm::vec2(selected_idx.x-1, selected_idx.y+1);
        intersect = false;
        while(  piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != piece_color(selected) &&
                intersect == false &&
                idx_check.x >= 0 && idx_check.y <= 7){
            if(piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != Color::none)
                intersect = true;
            valid_moves.push_back(glm::vec2(idx_check.x, idx_check.y));
            idx_check = glm::vec2(idx_check.x-1, idx_check.y+1);
        }

        // Look right,down 
        idx_check = glm::vec2(selected_idx.x+1, selected_idx.y+1);
        intersect = false;
        while(  piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != piece_color(selected) &&
                intersect == false &&
                idx_check.x <= 7 && idx_check.y <= 7){
            if(piece_color(bstate[int(idx_check.x)][int(idx_check.y)]) != Color::none)
                intersect = true;
            valid_moves.push_back(glm::vec2(idx_check.x, idx_check.y));
            idx_check = glm::vec2(idx_check.x+1, idx_check.y+1);
        }
    }


    ////////////////
    ///   King   ///
    ////////////////

    int castle_idx = -1;
    if(selected == Board::w_king || selected == Board::b_king){
        glm::vec2 idx_check[8];
        idx_check[0] = glm::vec2(selected_idx.x-1,selected_idx.y-1);
        idx_check[1] = glm::vec2(selected_idx.x  ,selected_idx.y-1);
        idx_check[2] = glm::vec2(selected_idx.x+1,selected_idx.y-1);
        idx_check[3] = glm::vec2(selected_idx.x-1,selected_idx.y+1);
        idx_check[4] = glm::vec2(selected_idx.x  ,selected_idx.y+1);
        idx_check[5] = glm::vec2(selected_idx.x+1,selected_idx.y+1);
        idx_check[6] = glm::vec2(selected_idx.x-1,selected_idx.y);
        idx_check[7] = glm::vec2(selected_idx.x+1,selected_idx.y);

        // Possible king moves
        for(int i = 0; i < 8; i++){
            // check if its in bounds, checks if it your own piece
            if(idx_check[i].x < 8 && idx_check[i].x >= 0 && 
               idx_check[i].y < 8 && idx_check[i].y >= 0 && 
               piece_color(bstate[int(idx_check[i].x)][int(idx_check[i].y)]) != piece_color(selected)){
                valid_moves.push_back(glm::vec2(idx_check[i].x, idx_check[i].y));
            }
        }
        // Castling
        if(selected == Board::w_king){
            // King side castle
            if(w_kside_castle && 
               bstate[5][7] == Board::empty &&
               bstate[6][7] == Board::empty &&
               tile_attackers(glm::vec2(5,7),Color::black).empty() &&
               tile_attackers(glm::vec2(6,7),Color::black).empty() ){
                valid_moves.push_back(glm::vec2(6, 7));
                castle_idx = valid_moves.size();
            }
            // Queen side castle
            if(w_qside_castle && 
               bstate[1][7] == Board::empty &&
               bstate[2][7] == Board::empty &&
               bstate[3][7] == Board::empty &&
               tile_attackers(glm::vec2(2,7),Color::black).empty() &&
               tile_attackers(glm::vec2(3,7),Color::black).empty() ){
                valid_moves.push_back(glm::vec2(2, 7));
                castle_idx = valid_moves.size();
            }
        }
        if(selected == Board::b_king){
            // King side castle
            if(b_kside_castle && 
               bstate[5][0] == Board::empty &&
               bstate[6][0] == Board::empty &&
               tile_attackers(glm::vec2(5,0),Color::white).empty() &&
               tile_attackers(glm::vec2(6,0),Color::white).empty() ){
                valid_moves.push_back(glm::vec2(6, 0));
                castle_idx = valid_moves.size();
            }
            // Queen side castle
            if(b_qside_castle && 
               bstate[1][0] == Board::empty &&
               bstate[2][0] == Board::empty &&
               bstate[3][0] == Board::empty &&
               tile_attackers(glm::vec2(2,0),Color::white).empty() &&
               tile_attackers(glm::vec2(3,0),Color::white).empty() ){
                valid_moves.push_back(glm::vec2(2, 0));
                castle_idx = valid_moves.size();
            }
        }
    }

    /*for(std::vector<int>::size_type i = 0; i != valid_moves.size(); i++){
        printf("Move List: (%1.0f,%1.0f)\n",valid_moves[i].x,valid_moves[i].y);
    }
    printf("=====\n");*/

    for(std::vector<int>::size_type i = 0; i != valid_moves.size(); i++){
        if(new_idx == valid_moves[i]){
            if(en_passant_idx-1 == static_cast<int>(i))
                *en_passant = true;
            if(selected == Board::b_pawn && new_idx.y == 7)
                *promote = true;
            if(selected == Board::w_pawn && new_idx.y == 0)
                *promote = true;
            if(castle_idx-1 == static_cast<int>(i))
                *castle = true;
            return true;
        }
    }
    return false;
}


// Creates a list of attackers (of a given color) for an input tile 
std::vector<glm::vec2> GameState::tile_attackers(glm::vec2 idx, Color color){
    // Generate a list of attackers
    std::vector<glm::vec2> attackers;

    // Save original state 
    Board orig_selected = selected;
    glm::vec2 orig_selected_idx(selected_idx);
    Board orig_bstate[8][8];
    for(int y = 0; y < 8; y++){
        for(int x = 0; x < 8; x++){
            orig_bstate[y][x] = bstate[y][x];
        }
    }
    // Put a fake piece on input tile so pawns can correctly attack it 
    if(color == white)
        bstate[int(idx.x)][int(idx.y)] = Board::b_pawn;
    else if(color == black)
        bstate[int(idx.x)][int(idx.y)] = Board::w_pawn;

    // Check all pieces to see if they can attack input tile
    bool a,b,c; 
    for(int y = 0; y < 8; y++){
        for(int x = 0; x < 8; x++){
            if(piece_color(bstate[x][y]) == color){
                selected = bstate[x][y];
                selected_idx = glm::vec2(x,y);
                if(movement(idx,&a,&b,&c)){ // Check if piece can move there
                    attackers.push_back(selected_idx);
                }
            }
        }
    }

    // Return to original state
    selected = orig_selected;
    selected_idx = glm::vec2(orig_selected_idx.x, orig_selected_idx.y);
    for(int y = 0; y < 8; y++){
        for(int x = 0; x < 8; x++){
            bstate[y][x] = orig_bstate[y][x];
        }
    }

    return attackers;
}
