#include <iostream>
#include "DS.h"
#include "Image.h"
#include "Camera.h"
#include "Models.h"
#include "Engine.h"

using namespace std;

int main() {
    const int width = 100;
    const int height = 100;
    Image i1{width,height};

    Camera cam(Ray(Point(0),Triple<float>(0,0,-1)),1,90);
    Sphere sp1(Point(0,0,-1),10,Color(1,0,0));
    Sphere sp2(Point(0,0,-5),2,Color(0,1,0));
    vector<Model*> models;
    models.push_back(&sp1);
    models.push_back(&sp2);

    RenderEngine render_man(cam,i1,models);
    render_man.render();
    render_man.writeImage("./sphere.ppm");

    return 0;
}