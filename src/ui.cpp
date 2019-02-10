#include <ui.h>

//////////////////////////////////
/// Resource Manager Functions ///
//////////////////////////////////

Resource_manager::Resource_manager(){
    // Load Shader
    shader = new Shader[1];
    this->shader[0].init("shader/shader.vs", "shader/shader.fs");

    // Load Texture
    texture = new Texture2D[2];
    int width, height, nrChannels;
    unsigned char *data;
    data = stbi_load("textures/square.png", &width, &height, &nrChannels, 0);
    this->texture[0].init(data, width, height, true);
    stbi_image_free(data);
    data = stbi_load("textures/ChessPiecesArray.png", &width, &height, &nrChannels, 0);
    this->texture[1].init(data, width, height, true);
    stbi_image_free(data);
}

Resource_manager::~Resource_manager(){
    delete [] shader;
    delete [] texture;
}



///////////////////////////
/// Partition Functions ///
///////////////////////////

Partition::Partition(){
    dimension = glm::vec2(0.0f, 0.0f);
}

Partition::~Partition(){
    // Default destructor
}

//void Partition::render(){
//    std::cout << "Render is not defined for general partition" << std::endl;
//}



/////////////////////
/// Box Functions ///
/////////////////////

Box::Box(Resource_manager *rm, glm::vec2 dimension, glm::vec3 color){

    this->shader = &(rm->shader[0]);
    this->texture = &(rm->texture[0]);
    this->dimension = dimension;
    this->color   = color;

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


Box::~Box(){
    // Default destructor
}



void Box::render(glm::vec2 coor){

    shader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(coor, 0.0f));
    model = glm::scale(model, glm::vec3(dimension.x/2, dimension.y/2, 1.0f));

    shader->SetMatrix4f("model", model);
    shader->SetVector3f("color", color);
    glActiveTexture(GL_TEXTURE0);
    texture->bind();
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

}


////////////////////////////////////
/// User Interface (UI) Functions //
////////////////////////////////////

UI::UI(GLFWwindow* window, int WIDTH, int HEIGHT){
    this->window = window;
    this->rm = new Resource_manager();
    this->resize_window(WIDTH, HEIGHT);

    // Create two partition of boxes
    glm::vec2 dimension;
    glm::vec3 color;


    //color = glm::vec3(0.0f,1.0f,0.0f);
    //dimension = glm::vec2(0.5f, 0.5f);
    box = new Box(&rm, dimension, color);
    //this->partition.push_back(new Box(&rm, dimension, color));
    //this->box.push_back(Box(&rm, dimension, color));
    //this->coor.push_back(glm::vec2(0.0f,0.0f));

    //color = glm::vec3(1.0f,0.0f,0.0f);
    //dimension = glm::vec2(0.5f, 0.5f);
    ////this->partition.push_back(new Box(&rm, dimension, color));
    //this->box.push_back(Box(&rm, dimension, color));
    //this->coor.push_back(glm::vec2(0.5f,0.5f));

}

UI::~UI(){

    delete box;
    delete rm;

}

void UI::resize_window(int width, int height){
    this->aspect_ratio = static_cast<float>(width)/static_cast<float>(height);
    this->WIDTH = width;
    this->HEIGHT = height;
    this->projection = glm::ortho( -this->aspect_ratio, this->aspect_ratio, 
                                   1.0f, -1.0f, 
                                  -1.0f, 1.0f);
    this->rm->shader[0].use();
    this->rm->shader[0].SetMatrix4f("projection", projection);
}

void UI::render(){

    this->rm->shader[0].use();
    this->rm->shader[0].SetMatrix4f("view", glm::mat4(1.0f));

    glm::vec3 color;
    color = glm::vec3(1.0f,1.0f,1.0f);
    this->tile.draw(this->rm->shader[0], this->rm->texture[0], 0.1f, color, glm::vec2(0.0f,0.0f) );

    //box->render(glm::vec2(0.0f,0.0f));
    //for(int i = 0; i < box.size(); i++){
    //    box[i].render(coor[i]);
    //}
}

