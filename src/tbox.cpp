#include <tbox.h>


// Loads ascii letters into a mesh texture and stores a VAO/VBO to access individual
// letters 
Text::Text(Resource_manager *rm){
    shader = &(rm->shader[1]);
    resize_text(48);
}

void Text::resize_text(unsigned int size){
    this->size = size; 
    this->size_f = size/800.0f; 

    // Generate textures
    FT_Library ft;
    FT_Face face;
    if(FT_Init_FreeType(&ft))
        std::cout << "FreeType::ERROR: Library init error" << std::endl;
    if(FT_New_Face(ft, "textures/fonts/arial.ttf", 0, &face))
        std::cout << "FreeType::ERROR: Failed to load font" << std::endl;
    
    FT_Set_Pixel_Sizes(face, 0, size);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

    // 96 [size]x[size] characters
    const int N = 96;
    char char_textures[N*size*size];  
    for (int i = 0; i < N*size*size; i++){
        char_textures[i] = 0;
    }

    int count = 0;
    for (unsigned int c = 32; c < 32+N; c++)
    {
        // Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph: " << c << std::endl;
            continue;
        }
        // Add character to texture mesh
        unsigned int w = face->glyph->bitmap.width;
        unsigned int h = face->glyph->bitmap.rows;
        for(int j = 0; j < h; j++){
            for(int i = 0; i < w; i++){
                char_textures[count*size*size + j*size + i] = face->glyph->bitmap.buffer[j*w+i];
            }
        }

        // Create VAO and VBO for indiviual character
        unsigned int VAO;       
        unsigned int VBO;      
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        // Calculate location vector
        float wf = (static_cast<float>(w)/size)/2.;    // width float 
        float hf = (static_cast<float>(h)/size)/2.;    // height float 
        float t_fraction = 1./static_cast<float>(N);
        float t_offset = count*t_fraction;
        float t_height = static_cast<float>(h)/size * t_fraction;
        float t_width = static_cast<float>(w)/size;
            
        float verticies[] = {
            // Loc     // TexCoord
            -wf, -hf,  0.0f,      t_offset, 
            -wf,  hf,  0.0f,      t_offset + t_height,
             wf,  hf,  t_width,   t_offset + t_height,

             wf,  hf,  t_width,   t_offset + t_height, 
             wf, -hf,  t_width,   t_offset, 
            -wf, -hf,  0.0f,      t_offset
        };

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindVertexArray(0);

        // Now store character for later use
        float bearing_x = (static_cast<float>(face->glyph->bitmap_left)/size)*size_f;
        // THis is not correct
        float bearing_y = (static_cast<float>(face->glyph->bitmap_top)/size - 
                           static_cast<float>(h)/size/2.) * size_f;
        float advance = static_cast<float>(face->glyph->advance.x >> 6)/size*size_f;
        
        std::cout << "Letter : " << static_cast<char>(c) << std::endl;
        std::cout << "Size   : " << wf << " , " << hf << std::endl;
        std::cout << "Bearing: " << bearing_x << " , " << bearing_y << std::endl;
        std::cout << "Advance: " << advance << std::endl;
        std::cout << "---------" << std::endl;

        Character character = {
            VAO,    // Vertex attributes
            VBO,    // Vertex buffer
            glm::vec2(bearing_x, bearing_y),
            advance
        };
        characters.insert(std::pair<GLchar, Character>(c, character));
        count += 1;
    }
    // Free freetype structures
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // Generate texture
    if( char_textures ) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RED,
            size, N*size, 0,  GL_RED,
            GL_UNSIGNED_BYTE,
            char_textures
        );

        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
}

void Text::render(std::string str, glm::vec2 pos, float size_i){

    shader->use();
    glm::vec3 color(1.0f,1.0f,1.0f);

    float x_offset = 0;
    for(int i = 0; i < str.length(); i++){
        Character ch = characters[str[i]];

        float xpos = pos.x + x_offset + ch.Bearing.x*size_i;
        float ypos = pos.y - ch.Bearing.y*size_i;
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(xpos, ypos, 0.0f));
        model = glm::scale(model, glm::vec3(size_i*size_f, size_i*size_f, 1.0f));

        shader->SetMatrix4f("model", model);
        shader->SetVector3f("color", color);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(ch.VAO);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        x_offset += ch.Advance*size_i;
    }
}

Tbox::Tbox(Text *t){
    text = t;
    pos = glm::vec2(-1.0f,0.0f);
    size = 1.0f;
}

void Tbox::render(){
    sentence = "THE QUICK BROWN FOX JUMPED OVER THE LAZY DOG";
    text->render(sentence, pos, size);
    sentence = "{the quick brown fox, jumped over the lazy dog}";
    text->render(sentence, glm::vec2(pos.x, pos.y+0.1f), size);
    sentence = "123456890-=!@#$%^&*()_+`~[]{};',./:\"<>?\\";
    text->render(sentence, glm::vec2(pos.x, pos.y+0.2f), size);
    sentence = "i i ii iii I I II III iAi IAI LllL";
    text->render(sentence, glm::vec2(pos.x, pos.y+0.3f), size);
}

/*
    // Generate textures
    FT_Library ft;
    FT_Face face;
    if(FT_Init_FreeType(&ft))
        std::cout << "FreeType::ERROR: Library init error" << std::endl;
    //if(FT_New_Face(ft, "textures/fonts/arial.ttf", 0, &face))
    if(FT_New_Face(ft, "textures/fonts/DejaVuSans.ttf", 0, &face))
        std::cout << "FreeType::ERROR: Failed to load font" << std::endl;
    
    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

    if (FT_Load_Char(face, 88, FT_LOAD_RENDER))
        std::cout << "Error" << std::endl;
    int size = face->glyph->bitmap.width*face->glyph->bitmap.rows;
    for(int i = 0; i < face->glyph->bitmap.rows; i++){
        for(int j = 0; j < face->glyph->bitmap.width; j++){
            if(face->glyph->bitmap.buffer[i*face->glyph->bitmap.width+j])
                std::cout << "0";
            else
                std::cout << " ";
        }
        std::cout << std::endl;
    }

    // 96 64x64 characters
    char char_textures[96*64*64];  
    unsigned int x_offset = 0;
    for (unsigned int c = 32; c < 128; c++)
    {
        // Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph: " << c << std::endl;
            continue;
        }
        unsigned int w = face->glyph->bitmap.width;
        unsigned int h = face->glyph->bitmap.rows;
        for(int j = 0; j < h; j++){
            for(int i = 0; i < w; i++){
            }
        }
        // Now store character for later use
        Character character = {
            VAO,
            VBO, 
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // Generate texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        face->glyph->bitmap.width,
        face->glyph->bitmap.rows,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        face->glyph->bitmap.buffer
    );

    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // Create VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*6*4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE, 4*sizeof(GLfloat),0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);


    //render function
    glm::vec3 color(1.0f,1.0f,1.0f);

    float x = pos.x; 
    float y = pos.y;
    std::string str("X");
    std::string::const_iterator c;
    c = str.begin();
    Character ch = Characters[*c];
    float xpos = x + ch.Bearing.x/800.0f*size;
    float ypos = y - (ch.Size.y/600.0f - ch.Bearing.y/600.0f) * size;
    float w = ch.Size.x/800.0f * size;
    float h = ch.Size.y/600.0f * size;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(xpos, ypos, 0.0f));
    model = glm::scale(model, glm::vec3(w, h, 1.0f));

    shader->SetMatrix4f("model", model);
    shader->SetVector3f("color", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    float verticies[] = {
         // Loc       // TexCoord
        -1.0f,  1.0f,  0.0f, 1.0f, 
         1.0f,  1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
                    
        -1.0f, -1.0f,  1.0f, 1.0f, 
        -1.0f,  1.0f,  0.0f, 1.0f, 
         1.0f, -1.0f,  1.0f, 0.0f
    };
    glBindTexture(GL_TEXTURE_2D, ch.TextureID);
    //texture->bind();
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferSubData(GL_ARRAY_BUFFER,0, sizeof(verticies), verticies);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

*/
