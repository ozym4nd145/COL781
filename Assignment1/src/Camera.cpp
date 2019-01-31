#include <iostream>
#include <optional>

#include "Camera.h"
#include "DS.h"
#include "defs.h"
#include "utils.h"

Ray Camera::transformCameraToWorld(const Ray& r) const {
    auto new_src = apply_transformation(r.src, _transformation);
    auto new_dest =
        apply_transformation(r.src + r.length * r.dir, _transformation);
    return Ray(new_src, new_dest - new_src);
}

std::optional<Ray> Camera::getRay(float i, float j) const {
    if (i > 1.0 || j > 1.0 || i < 0 || j < 0) return {};  // outside range
    float x = 2 * i - 1;                                  // [-1,1]
    float y = 1 - 2 * j;                                  // [-1,1]
    float x_corr = x * _x_correction;  // [-tan(theta/2),tan(theta/2)]
    float y_corr = y * _y_correction;  // [-tan(theta/2)/ar,tan(theta/2)/ar]
    Ray r(Vector3f::Zero(), Vector3f(x_corr, y_corr, -1));
    return transformCameraToWorld(r);
}

std::ostream& operator<<(std::ostream& os, const Camera& cam) {
    return os << "Camera{aspect_ratio=" << cam._ar << ",fov=" << cam._fov_degree
              << ",transformation=" << cam._transformation << "}";
}