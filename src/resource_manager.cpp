#include <resource_manager.h>

//////////////////////////////////
/// Resource Manager Functions ///
//////////////////////////////////

Resource_manager::Resource_manager(){
    std::cout << "Loading Shaders...";
    // Load Shader
    shader = new Shader[1];
    this->shader[0].init("shader/shader.vs", "shader/shader.fs");
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
}

Resource_manager::~Resource_manager(){
    delete [] shader;
}
