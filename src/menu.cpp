#include <menu.h>

/////////////////////
// Menu Primitives //
/////////////////////

Rectangle::Rectangle(Resource_manager *rm){

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

void Rectangle::render(glm::vec3 color, glm::vec2 pos, glm::vec2 size){

    // Render tile
    shader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos, 0.0f));
    model = glm::scale(model, glm::vec3(size.x/2, size.y/2, 1.0f));

    shader->SetMatrix4f("model", model);
    shader->SetVector3f("color", color);
    glActiveTexture(GL_TEXTURE0);
    texture->bind();
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}


Menu::Menu(Resource_manager *rm){
    // Text

    // Background
    background = new Rectangle(rm);

    // Buttons
    
}

Menu::~Menu(){
    delete background;
}

void Menu::render(float aspect_ratio){

    // Background
    glm::vec2 dim(1.0f-0.2f, 2.0f-0.4f); 
    glm::vec3 bg_color(1.0f, 1.0f, 1.0f);
    float x = -aspect_ratio + 2.0f + dim.x/2;
    background->render(bg_color, glm::vec2(x, 0), dim); 
}
