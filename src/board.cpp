#include <board.h>

//////////////////////
/// Tile Functions ///
//////////////////////

Tile::Tile(Resource_manager *rm){

    shader = &(rm->shader[0]);
    texture = &(rm->texture[0]);

    unsigned int VBO;

    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);

    float vertices[] = {
         // Loc       // TexCoord
        -1.0f,  1.0f,  0.01f, 0.99f, 
         1.0f,  1.0f,  0.01f, 0.01f,
         1.0f, -1.0f,  0.99f, 0.01f,

        -1.0f, -1.0f,  0.99f, 0.99f, 
        -1.0f,  1.0f,  0.01f, 0.99f, 
         1.0f, -1.0f,  0.99f, 0.01f
    };

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);  
    glBindVertexArray(0);
}

void Tile::render(glm::vec3 color, glm::vec2 pos, float size){

    shader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos, 0.0f));
    model = glm::scale(model, glm::vec3(size/2, size/2, 1.0f));

    shader->SetMatrix4f("model", model);
    shader->SetVector3f("color", color);
    glActiveTexture(GL_TEXTURE0);
    texture->bind();
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

}

///////////////////////
/// Piece Functions ///
///////////////////////

Piece::Piece(Resource_manager *rm, int x_offset, int y_offset){

    shader = &(rm->shader[0]);
    texture = &(rm->texture[1]);

    unsigned int VBO;

    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);

    float y_tex = y_offset*0.5f; 
    float x_tex = x_offset/6.0f; 
    float x_step = 1.0f/6.0f; 

    float vertices[] = {
         // Loc       // TexCoord
        -1.0f,  1.0f,  x_tex,        y_tex+0.5f,
         1.0f,  1.0f,  x_tex+x_step, y_tex+0.5f,
         1.0f, -1.0f,  x_tex+x_step, y_tex,  

        -1.0f, -1.0f,  x_tex,        y_tex,
        -1.0f,  1.0f,  x_tex,        y_tex+0.5f,   
         1.0f, -1.0f,  x_tex+x_step, y_tex  
    };

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);  
    glBindVertexArray(0);
}

void Piece::render(glm::vec2 pos, float size){
    glm::vec3 color(1.0f,1.0f,1.0f);

    shader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos, 0.0f));
    model = glm::scale(model, glm::vec3(size/2, size/2, 1.0f));

    shader->SetMatrix4f("model", model);
    shader->SetVector3f("color", color);
    glActiveTexture(GL_TEXTURE0);
    texture->bind();
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}


///////////////////////
/// Board Functions ///
///////////////////////

Board::Board(Resource_manager *rm){

    // Visuals
    coor = glm::vec2(0.2f,0.2f);
    dim = 2.0f - 0.2f*2;

    // Tile Color arr
    color_arr[0] = glm::vec3(0.2f,0.2f,0.2f); // black
    color_arr[1] = glm::vec3(0.8f,0.8f,0.8f); // white 

    color_arr[2] = glm::vec3(0.8f,0.8f,0.8f); // highlighted 
    color_arr[3] = glm::vec3(0.4f,0.4f,1.0f); // selected 
    color_arr[4] = glm::vec3(0.6f,0.6f,1.0f); // played from 
    color_arr[5] = glm::vec3(1.0f,0.2f,0.2f); // check 
    color_arr[6] = glm::vec3(1.0f,1.0f,1.0f); // none 

    // Create tiles and Pieces array
    tile = new Tile(rm); 
    float x_offset, y_offset;
    for(int y = 0; y < 2; y++){
        for(int x = 0; x < 6; x++){
            pieces.push_back(Piece(rm, x, y));
        } 
    }

    // Board state initialization
    for(int y = 0; y < 8; y++){
        for(int x = 0; x < 8; x++){
            bstate[x][y] = TS::e; // empty
            bcolor[x][y] = TC::none; // empty
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

    // Game state 
    selected = TS::e;
    selected_idx = glm::vec2(-1.0f, -1.0f);
}

Board::~Board(){
    delete tile;
}

void Board::render(glm::vec2 mpos, float aspect_ratio){

    float tile_size = dim/8;

    float x_initial = -aspect_ratio + coor.x + tile_size/2;
    float y_initial = -1.0f + coor.y + tile_size/2;

    glm::vec2 pos(x_initial, y_initial);

    int t = 1;
    // Paint Tiles
    for(int y = 0; y < 8; y++){
        for(int x = 0; x < 8; x++){
            // Paint tiles and pieces
            int c_idx = static_cast<int> (bcolor[x][y]);
            glm::vec3 tc(color_arr[t].x*color_arr[c_idx].x,
                         color_arr[t].y*color_arr[c_idx].y,
                         color_arr[t].z*color_arr[c_idx].z);
            tile->render(tc, pos, tile_size);

            int piece_idx = static_cast<int> (bstate[x][y]);
            if(piece_idx != TS::e){
                pieces[piece_idx].render(pos, tile_size);
            }

            // Increment position
            t = !t;
            pos = glm::vec2(pos.x + tile_size, pos.y);
        }
        pos = glm::vec2(x_initial, pos.y + tile_size);
        t = !t;
    }

    // Draw Selected Piece
    if(selected != TS::e){
        int piece_idx = static_cast<int> (selected);
        pieces[selected].render(mpos, tile_size);
    }
}

void Board::board_right_click(glm::vec2 mpos){
    int xidx, yidx;
    get_board_idx(mpos, &xidx, &yidx);
    if(xidx != -1 && yidx != -1){
        if(bcolor[xidx][yidx] != highlight)
            bcolor[xidx][yidx] = TC::highlight;
        else
            bcolor[xidx][yidx] = TC::none;
    }
}


void Board::board_left_click(glm::vec2 mpos, bool down){
    int xidx, yidx;
    get_board_idx(mpos, &xidx, &yidx);
    // Left click press 
    if(down){    
        if(xidx != -1 && yidx != -1){
            selected_idx = glm::vec2(xidx, yidx);
            selected = bstate[xidx][yidx];
            bstate[xidx][yidx] = TS::e;
            bcolor[xidx][yidx] = TC::tile_select;
        }
    }
    // Left click release 
    else{
        if(this->selected != TS::e){ 
            if(xidx == -1 || yidx == -1){
                int old_idx_x = static_cast<int>(selected_idx.x);
                int old_idx_y = static_cast<int>(selected_idx.y);
                bstate[old_idx_x][old_idx_y] = TS::e;
            }else{
                bstate[xidx][yidx] = selected;
            }
            selected = TS::e;
            bcolor[xidx][yidx] = TC::none;
        }
    }
}

void Board::get_board_idx(glm::vec2 mpos, int *x, int *y){
    float xposd = mpos.x;
    float yposd = mpos.y;

    // Check if a piece was selected 
    xposd = floor(xposd/(dim/8));
    yposd = floor(yposd/(dim/8));

    if(xposd > 7 || xposd < 0)
        xposd = -1;
    if(yposd > 7 || yposd < 0)
        yposd = -1;

    *x = static_cast<int>(xposd);
    *y = static_cast<int>(yposd);
}


