#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// GLM Libraries
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Move pair
class MP{
public:
    glm::ivec2 from;
    glm::ivec2 to;
};

// PGN interpreter
class PGN{
public:
    std::string result;
    //std::vector<MP>;
};

int main(){
    std::ifstream f; 
    f.open("g0");
    if(!f.is_open()){
        std::cout << "Failed to open file" << std::endl;
    }
    char buf[256];
    std::vector<std::string> lines;
    while(f.getline(buf,256)){
        lines.push_back(std::string(buf));
        //std::cout << buf << std::endl;
    }
    for(std::vector<std::string>::iterator it=lines.begin(); it != lines.end(); ++it){
        std::cout << *it << std::endl;
    }
    

    f.close();
    return 0;
}

// Ideas for piece movement
/*
 * updates oldboard with new piece at location. 
 * find candidate pieces on old board to make the movement
 * remove piece from initial position
 *
 */

