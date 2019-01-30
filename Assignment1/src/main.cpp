#include <iostream>
#include "DS.h"
#include "Image.h"
#include "Camera.h"
#include "Models.h"
#include "Engine.h"

using namespace std;

typedef Triple<float> Vector3f;

int main() {
    const int width = 1000;
    const int height = 1000;
    Image i1{width,height};

    IllumParams ip1,ip2, ip3;
    ip1.Ka = Vector3f(0.9,0.1,0.1);
    ip1.Kd = Vector3f(0.9,0.1,0.1);
    ip1.Ks = Vector3f(0.9,0.1,0.1);
    ip1.Krg = Vector3f(0.9,0.1,0.1);
    ip1.Ktg = Vector3f(0.9,0.1,0.1);

    ip2.Ka = Vector3f(0,0,0);
    ip2.Kd = Vector3f(0,0,0);
    ip2.Ks = Vector3f(0,0,0);
    ip2.Krg = Vector3f(0,0,0);
    ip2.Ktg = Vector3f(1,1,1);

    ip3.Ka = Vector3f(1,1,1);
    ip3.Kd = Vector3f(0.7,0.7,0.7);
    ip3.Ks = Vector3f(0.5,0.5,0.5);
    ip3.Krg = Vector3f(1,1,1);
    ip3.Ktg = Vector3f(1,1,1);


    Camera cam(Ray(Point(0),Triple<float>(0,0,-1)),1,90);
    // Sphere sp3(Point(-0.5,-0.5,-2),1,ip1);
    Sphere sp3(Point(-1,0,-6),1,ip1);
    // Sphere sp1(Point(-1,0,-3),0.7,ip2,1);
    Sphere sp1(Point(0,0,-6),1.5,ip2,1);
    Sphere sp2(Point(1,0,-6),1,ip3);
    // Sphere sp2(Point(0,0,-5),2,ip2);
    // Sphere sp2(Point(0,0,-5),2,ip3);
    // Sphere sp2(Point(0,0,-10),2,ip3);
    // Light lg1(Point(9,9,9),Color(1,1,1));
    // Light lg1(Point(0,0,-4),Color(1,1,1));
    Light lg2(Point(0,0,10),Color(1,1,1));
    vector<Model*> models;
    models.push_back(&sp1);
    models.push_back(&sp2);
    models.push_back(&sp3);

    vector<Light*> lights;
    // lights.push_back(&lg1);
    lights.push_back(&lg2);

    RenderEngine render_man(cam,i1,models,lights,Color(0.2,0.2,0.2));
    // RenderEngine render_man(cam,i1,models,lights,Color(0,0,0));
    render_man.render();
    render_man.writeImage("./sphere.ppm");

    return 0;
}