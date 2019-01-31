#include <iostream>
#include "DS.h"
#include "Image.h"
#include "Camera.h"
#include "Models.h"
#include "Engine.h"
#include "utils.h"

using namespace std;

int main() {
    const int width = 512;
    const int height = 512;
    Image i1{width,height};

    Material ivory, glass, red_rubber, blue_rubber, mirror;

    ivory.Ka = Vector3f::Constant(0);
    ivory.Kd = 0.6*Vector3f(0.4,0.4,0.3);
    ivory.Ks = Vector3f::Constant(0.3);
    ivory.Krg = Vector3f::Constant(0.1);
    ivory.Ktg = Vector3f::Constant(0);
    ivory.refractive_index = 1.0;
    ivory.specular_coeff = 50.0;

    glass.Ka = Vector3f::Constant(0);
    glass.Kd = 0*Vector3f(0.6,0.7,0.8);
    glass.Ks = Vector3f::Constant(0.5);
    glass.Krg = Vector3f::Constant(0.1);
    glass.Ktg = Vector3f::Constant(0.8);
    glass.refractive_index = 1.5; // TODO: 1.5
    glass.specular_coeff = 125.0;

    red_rubber.Ka = Vector3f::Constant(0);
    red_rubber.Kd = 0.9*Vector3f(0.3,0.1,0.1);
    red_rubber.Ks = Vector3f::Constant(0.1);
    red_rubber.Krg = Vector3f::Constant(0);
    red_rubber.Ktg = Vector3f::Constant(0);
    red_rubber.refractive_index = 1.0;
    red_rubber.specular_coeff = 10.0;

    blue_rubber.Ka = Vector3f::Constant(0);
    blue_rubber.Kd = 0.9*Vector3f(0.1,0.1,0.3);
    blue_rubber.Ks = Vector3f::Constant(0.1);
    blue_rubber.Krg = Vector3f::Constant(0);
    blue_rubber.Ktg = Vector3f::Constant(0);
    blue_rubber.refractive_index = 1.0;
    blue_rubber.specular_coeff = 10.0;

    mirror.Ka = Vector3f::Constant(0);
    mirror.Kd = 0*Vector3f::Constant(1);
    mirror.Ks = Vector3f::Constant(10);
    mirror.Krg = Vector3f::Constant(0.8);
    mirror.Ktg = Vector3f::Constant(0);
    mirror.refractive_index = 1.0;
    mirror.specular_coeff = 1425;

    Matrix4f world_camera=Matrix4f::Identity();
    world_camera(3,0) = 1;
    world_camera(3,1) = 1;
    world_camera(3,2) = 0;
    cout<<"WorldCamera Matrix:\n"<<world_camera<<endl;

    Camera cam(world_camera,1.0,90.0);
    
    Sphere sp1(Point(-3,0,-16),2,ivory);
    Sphere sp2(Point(-1,-1.5,-12),2,glass);
    Sphere sp3(Point(1.5,-0.5,-18),3,red_rubber);
    Sphere sp4(Point(7,5,-18),4,mirror);

    Plane pl1(Ray(Point(0,-4,0),Vector3f(0,1,0)),blue_rubber);

    Triangle tr1(Point(2,1,-10),Point(-1,1,-11),Point(0,4,-12),mirror);

    vector<Model*> models;
    models.push_back(&sp1);
    models.push_back(&sp2);
    models.push_back(&sp3);
    models.push_back(&sp4);
    models.push_back(&pl1);
    models.push_back(&tr1);

    Light lg1(Point(-20,20,20),Color::Constant(1.5));
    Light lg2(Point(30,50,-25),Color::Constant(1.8));
    Light lg3(Point(30,20,30),Color::Constant(1.7));

    vector<Light*> lights;
    lights.push_back(&lg1);
    lights.push_back(&lg2);
    lights.push_back(&lg3);

    RenderEngine render_man(cam,i1,models,lights,Color(0.2,0.2,0.2));
    // RenderEngine render_man(cam,i1,models,lights,Color(0,0,0));
    render_man.render();
    render_man.writeImage("./sphere.ppm");

    return 0;
}