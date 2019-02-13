#include <iostream>
#include <optional>
#include "DS.h"
#include "Models.h"
#include "defs.h"

std::optional<std::pair<float, const Model*>>
Sphere::_getIntersectionLengthAndPart(const Ray& r) const {
    // TODO: check intersection inside Sphere
    Ray src_center(r.src, _center - r.src);
    const float cos_theta = src_center.dir.dot(r.dir);
    if (src_center.length > _radius && cos_theta <= 0) {
        return {};  // ray can't intersect
    }

    const float src_center_sq = src_center.length * src_center.length;
    const float src_center_proj = src_center.length * cos_theta;
    const float center_ray_sq =
        src_center_sq - src_center_proj * src_center_proj;

    if (center_ray_sq > _radius_sq) return {};  // not intersecting
    const float d = sqrt(_radius_sq - center_ray_sq);
    const float t0 = src_center_proj + d;
    const float t1 = src_center_proj - d;

    float min_dist = std::min(t0, t1);
    float max_dist = std::max(t0, t1);
    float dist = (min_dist < 0) ? max_dist : min_dist;  // use correct distance
    return std::make_pair(dist, this);
}

bool Sphere::_isOnSurface(const Point& p) const {
    return (fabs((p - _center).norm() - _radius) <= EPSILON);
}

// returns outward normal
std::optional<Ray> Sphere::_getNormal(const Point& p) const {
    if (!_isOnSurface(p)) return {};
    auto normal = Ray(p, p - _center);
    return normal;
};

std::ostream& Sphere::print(std::ostream& os) const {
    return os << "Sphere{center=" << _center << ",radius=" << _radius << "}";
}
