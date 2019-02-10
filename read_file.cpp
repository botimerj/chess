#include <iostream>
#include <fstream>
#include <cstdlib> 
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


using namespace std;

class UI_config{
public:
    string name;
    glm::vec2 dim;
    glm::vec2 coor;

    UI_config();
};

UI_config::UI_config(){
    name = string("Not configured");
    dim = glm::vec2(0.0f,0.0f);
    coor = glm::vec2(0.0f,0.0f);
}

class Global_parameters{
public:
    //UI_config board;
    UI_config board_config;
    Global_parameters(string file_name);
};
 
Global_parameters::Global_parameters(string file_name){
    ifstream file;
    char buf[256];
    file.open(file_name);
    if(file.is_open()){
        while(file.getline(buf,256)){
            string tmp(buf);

            if(tmp == string("[Board]")){ 
                board_config.name = string("board");
            }
            file.getline(buf,256);
            string dim(buf);
            string xdim(dim.substr( dim.find("'")+1,3));
            string ydim(dim.substr( dim.find(",")+1,3));
            board_config.dim = glm::vec2(xdim,ydim);
            file.getline(buf,256);
            string dim(buf);
            string xdim(dim.substr( dim.find("'")+1,3));
            string ydim(dim.substr( dim.find(",")+1,3));
            board_config.dim = glm::vec2(xdim,ydim);
        }
    }
}




int main(){
    Global_parameters gp(string("config.ini"));

    return 0;
}
