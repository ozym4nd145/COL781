#pragma once

#include <math.h>
#include <iostream>
#include "utils.h"

class Ray {
   public:
    const Point src;
    const Vector3f dir;
    const float length;
    Ray(const Point& source, const Vector3f& direction)
        : src{source}, dir{direction.normalized()}, length{direction.norm()} {}
    friend std::ostream& operator<<(std::ostream& os, const Ray& r) {
        return (os << "Ray{" << r.src << " -> " << r.dir << "}");
    }
};

struct QuadricParams {
    float A, B, C, D, E, F, G, H, I, J;
    QuadricParams(const vector<float>& qp) {
        assert(qp.size() == 10);
        A = qp[0];
        B = qp[1];
        C = qp[2];
        D = qp[3];
        E = qp[4];
        F = qp[5];
        G = qp[6];
        H = qp[7];
        I = qp[8];
        J = qp[9];
    }
};

struct Material {
    Vector3f Ka, Kd, Ks, Krg, Ktg;
    float refractive_index, specular_coeff;
    Material()
        : Ka{Vector3f::Zero()},
          Kd{Vector3f::Zero()},
          Ks{Vector3f::Zero()},
          Krg{Vector3f::Zero()},
          Ktg{Vector3f::Zero()},
          refractive_index{-1},
          specular_coeff{1} {}
    Material(const Vector3f& ka, const Vector3f& kd, const Vector3f& ks,
             const Vector3f& krg, const Vector3f& ktg, float ri, float sc)
        : Ka{ka},
          Kd{kd},
          Ks{ks},
          Krg{krg},
          Ktg{ktg},
          refractive_index{ri},
          specular_coeff{sc} {}
};

// struct State{
//     vector<Model*> models;
//     vector<Light*> lights;
//     unordered_map<string,Material*> materials;
//     Camera *cam;
// };