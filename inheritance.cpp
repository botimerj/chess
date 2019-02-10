#include <iostream> 
#include <vector> 

using namespace std;


class Shape{
public:
    Shape();
    ~Shape();

    int vert;
    virtual void show();
};

Shape::Shape(){
    vert = 0;
}

Shape::~Shape(){}

void Shape::show(){
    cout << "Shape has " << vert << " verticies" << endl;
}


class Triangle : public Shape{
public:
    Triangle();
    ~Triangle();

    void show(); 
};

Triangle::Triangle(){
    vert = 3;
}

Triangle::~Triangle(){}

void Triangle::show(){
    cout << "Triangle has " << vert << " verticies" << endl;
}

int main(){
    //Shape shape;
    Triangle t1;

    //shape.show();
    t1.show();

    vector<Shape *> shape_vector;

    shape_vector.push_back( new Shape );
    shape_vector.push_back( new Triangle );
    for(int i = 0; i < shape_vector.size(); i++){
        shape_vector[i]->show();
    }

    return 0;
}
