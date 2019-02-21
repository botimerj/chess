#include <resource_manager.h>

//////////////////////////////////
/// Resource Manager Functions ///
//////////////////////////////////

Resource_manager::Resource_manager(int width_i){
    std::cout << "Loading Shaders...";
    // Load Shader
    shader = new Shader[2];
    this->shader[0].init("shader/shader.vs", "shader/shader.fs");
    this->shader[1].init("shader/shader.vs", "shader/shader_fonts.fs");
    std::cout << "Success!" << std::endl;

    // Load Texture
    std::cout << "Loading Textures...";
    texture = new Texture2D[2];
    int width, height, nrChannels;
    unsigned char *data;
    data = stbi_load("textures/square.png", &width, 
                                            &height, 
                                            &nrChannels, 0);
    this->texture[0].init(data, width, height, true);
    stbi_image_free(data);

    data = stbi_load("textures/ChessPiecesArray.png", &width, 
                                                      &height, 
                                                      &nrChannels, 0);
    this->texture[1].init(data, width, height, true);
    stbi_image_free(data);
    std::cout << "Success!" << std::endl;

    // Load fonts 
    text   = new Text(shader, width_i, "textures/fonts/arial.ttf");
    textb  = new Text(shader, width_i, "textures/fonts/arialbd.ttf");
    texti  = new Text(shader, width_i, "textures/fonts/ariali.ttf");
    textbi = new Text(shader, width_i, "textures/fonts/arialbi.ttf");


}

Resource_manager::~Resource_manager(){
    delete [] shader;
    delete text;
    delete textb;
    delete texti;
    delete textbi;
}
