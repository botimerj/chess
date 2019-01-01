
#include <piece.h>

Piece::Piece(int x_offset, int y_offset){
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

    this->color = glm::vec3(1.0f,1.0f,1.0f);
}

Piece::~Piece(){
    //glDeleteVertexArrays(1, &this->VAO);
}

void Piece::draw(Shader &shader, Texture2D &texture, float size, glm::vec2 pos){ 
    
    shader.use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos, 0.1f));
    //model = glm::rotate(model, glm::radians(this->rot), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(size/2, size/2, 1.0f));

    shader.SetMatrix4f("model", model);
    shader.SetVector3f("color", this->color);
    glActiveTexture(GL_TEXTURE0);
    texture.bind();
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
