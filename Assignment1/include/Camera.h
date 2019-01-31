#pragma once

#include <iostream>
#include <optional>
#include "DS.h"
#include "utils.h"

class Camera {
   private:
    const Matrix4f _transformation;
    const float _ar;          // aspect ratio
    const float _fov_degree;  // field of view in degrees
    float _x_correction;      // correction factor for x
    float _y_correction;      // correction factor for y

    Ray transformCameraToWorld(const Ray& r) const {
        auto new_src = apply_transformation(r.src, _transformation);
        auto new_dir = apply_transformation(r.dir, _transformation);
        return Ray(new_src, new_dir * r.length);
    }

   public:
    Camera(const Matrix4f& trans, float ar, float fov_degree)
        : _transformation(trans), _ar{ar}, _fov_degree{fov_degree} {
        // convert field of view to radians
        _x_correction = tan((_fov_degree * PI) / 360.0);  // tan(theta/2);
        _y_correction = _x_correction / _ar;
    }

    /**
     * i => [0,1] image x
     * j => [0,1] image y
     */
    std::optional<Ray> getRay(float i, float j) const {
        if (i > 1.0 || j > 1.0 || i < 0 || j < 0) return {};  // outside range
        float x = 2 * i - 1;                                  // [-1,1]
        float y = 1 - 2 * j;                                  // [-1,1]
        float x_corr = x * _x_correction;  // [-tan(theta/2),tan(theta/2)]
        float y_corr = y * _y_correction;  // [-tan(theta/2)/ar,tan(theta/2)/ar]
        Ray r(Vector3f::Zero(), Vector3f(x_corr, y_corr, -1));
        return transformCameraToWorld(r);
    }

    friend std::ostream& operator<<(std::ostream& os, const Camera& cam) {
        return os << "Camera{aspect_ratio=" << cam._ar
                  << ",fov=" << cam._fov_degree
                  << ",transformation=" << cam._transformation << "}";
    }
};