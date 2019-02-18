#pragma once

#include <iostream>
#include <optional>
#include "DS.h"
#include "defs.h"

class Camera {
   private:
    const Matrix4f _transformation;
    const float _ar;          // aspect ratio
    const float _fov_degree;  // field of view in degrees
    float _x_correction;      // correction factor for x
    float _y_correction;      // correction factor for y

    Ray transformCameraToWorld(const Ray& r) const;

   public:
    Camera(const Matrix4f& trans, float ar, float fov_degree)
        : _transformation(trans), _ar{ar}, _fov_degree{fov_degree} {
        // convert field of view to radians
        _x_correction = tan((_fov_degree * PI) / 360.0);  // tan(theta/2);
        _y_correction = _x_correction / _ar;
    }
    std::optional<Ray> getRay(float i, float j) const;
    friend std::ostream& operator<<(std::ostream& os, const Camera& cam);
    const Matrix4f& getTransformation() const { return _transformation; }
};