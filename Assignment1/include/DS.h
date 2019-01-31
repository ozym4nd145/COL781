#pragma once

#include <math.h>
#include <iostream>
#include "utils.h"

class Ray {
   public:
    Point src;
    Vector3f dir;
    float length;
    Ray(const Point& source, const Vector3f& direction)
        : src{source}, dir{direction.normalized()}, length{direction.norm()} {}
    friend std::ostream& operator<<(std::ostream& os, const Ray& r) {
        return (os << "Ray{" << r.src << " -> " << r.dir << "}");
    }
};

struct QuadricParams {
    float A, B, C, D, E, F, G, H, I, J;
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
};