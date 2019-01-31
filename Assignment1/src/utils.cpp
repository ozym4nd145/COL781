#include "utils.h"
#include <Eigen/Core>
#include "Camera.h"
#include "DS.h"
#include "Models.h"
#include "defs.h"

using json = nlohmann::json;
using namespace std;

Vector4f augment(const Vector3f &vec, float val) {
    Vector4f new_vec;
    new_vec.head(3) = vec;
    new_vec[3] = val;
    return new_vec;
}

Vector3f apply_transformation(const Vector3f &point, const Matrix4f &trans) {
    Vector4f p_temp = augment(point, 1.0);
    Vector4f new_point = p_temp * trans;
    new_point[0] /= new_point[3];
    new_point[1] /= new_point[3];
    new_point[2] /= new_point[3];
    return Vector3f(new_point.head(3));
}

std::optional<std::pair<float, float>> solve_quadratic(float a, float b,
                                                       float c) {
    if (a == 0) {
        if (b == 0) {
            if (c == 0) return std::make_pair(c, c);
            return {};
        } else
            return std::make_pair(-c / b, -c / b);
    }
    float D = b * b - (4.0 * a * c);
    if (D < 0) return {};
    float sqrt_D = sqrt(D);
    float x1 = (-b - sqrt_D) * 0.5 / a;
    float x2 = (-b + sqrt_D) * 0.5 / a;
    return std::make_pair(std::min(x1, x2), std::max(x1, x2));
}

Vector3f get_vector3f(const json &j) {
    float x = j[0], y = j[1], z = j[2];
    return Vector3f(x, y, z);
}

Model *parse_model(const json &j,
                   unordered_map<string, Material *> &materials) {
    string type = j["type"];
    string mat = j["material"];
    Model *m = NULL;
    if (materials.find(mat) == materials.end()) return m;
    if (type == "sphere") {
        Point center = get_vector3f(j["center"]);
        float radius = j["radius"];
        m = new Sphere(center, radius, *materials[mat]);
    } else if (type == "plane") {
        Point ray_src = get_vector3f(j["ray_src"]);
        Vector3f ray_normal = get_vector3f(j["ray_normal"]);
        m = new Plane(Ray(ray_src, ray_normal), *materials[mat]);
    } else if (type == "quadric") {
        QuadricParams qp(j["qp"]);
        m = new Quadric(qp, *materials[mat]);
    } else if (type == "triangle") {
        Point p1 = get_vector3f(j["p1"]);
        Point p2 = get_vector3f(j["p2"]);
        Point p3 = get_vector3f(j["p3"]);
        m = new Triangle(p1, p2, p3, *materials[mat]);
    } else if (type == "collection") {
        json cj = j["elements"];
        Collection *coll = new Collection(*materials[mat]);
        for (auto &el : cj) {
            Model *temp = parse_model(el, materials);
            coll->addModel(temp);
        };
        m = &(*coll);
    } else if (type == "box") {
        Point center = get_vector3f(j["center"]);
        Point x_axis = get_vector3f(j["x_axis"]);
        Point y_axis = get_vector3f(j["y_axis"]);
        float length = j["length"];
        float breadth = j["breadth"];
        float height = j["height"];
        m = new Box(center, x_axis, y_axis, length, breadth, height,
                    *materials[mat]);
    }
    return m;
}

void get_models(const json &j, vector<Model *> &models,
                unordered_map<string, Material *> &materials) {
    if (j.find("models") == j.end()) return;
    json jm = j["models"];
    for (auto &el : jm) {
        Model *m = parse_model(el, materials);
        if (m != NULL) models.push_back(m);
    }
}

pair<string, Material *> parse_material(const json &j) {
    string name = j["name"];
    Vector3f Ka = get_vector3f(j["Ka"]);
    Vector3f Kd = get_vector3f(j["Kd"]);
    Vector3f Ks = get_vector3f(j["Ks"]);
    Vector3f Krg = get_vector3f(j["Krg"]);
    Vector3f Ktg = get_vector3f(j["Ktg"]);
    float refractive_index = j["ri"], specular_coeff = j["sc"];
    Material *m =
        new Material(Ka, Kd, Ks, Krg, Ktg, refractive_index, specular_coeff);
    return std::make_pair(name, m);
}

void get_materials(const json &j,
                   unordered_map<string, Material *> &materials) {
    if (j.find("materials") == j.end()) return;
    json jm = j["materials"];
    for (auto &el : jm) {
        pair<string, Material *> m = parse_material(el);
        materials[m.first] = m.second;
    }
}

Light *parse_light(const json &j) {
    Vector3f loc = get_vector3f(j["loc"]);
    Vector3f intensity = get_vector3f(j["intensity"]);
    return new Light(loc, intensity);
}

void get_lights(const json &j, vector<Light *> &lights) {
    json lj = j["lights"];
    for (auto &el : lj) {
        lights.push_back(parse_light(el));
    }
}

Camera *get_camera(const json &j) {
    json jc = j["camera"];
    float ar = jc["ar"];
    float fov = jc["fov"];
    vector<float> trans_vec = jc["trans"];
    assert(trans_vec.size() == 16);
    Matrix4f trans_matrix(trans_vec.data());
    Camera *c = new Camera(trans_matrix, ar, fov);
    return c;
}

State get_state(string filename) {
    std::ifstream ifile(filename);
    json j;
    ifile >> j;
    unordered_map<string, Material *> materials;
    vector<Model *> models;
    vector<Light *> lights;
    Camera *cam;
    get_materials(j, materials);
    get_models(j, models, materials);
    get_lights(j, lights);
    cam = get_camera(j);
    State s = {models, lights, materials, cam};
    return s;
}
