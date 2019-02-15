#include <iostream>
#include <optional>
#include <cmath>
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

std::optional<Color> Sphere::_getTexture(const Point& p) const {
    if((this->mat).img == NULL) return {};

    auto direction = (p-_center).normalized();
    float u = 0.5 + atan2(direction[2],direction[0])/(2*PI);
    float v = 0.5 - asin(direction[1])/PI;

    auto& img = *((this->mat).img);

    int w = u*img.width();
    int h = v*img.height();

    float r = (float)(*img.data(w,h,0,0))/255.0;
    float g = (float)(*img.data(w,h,0,1))/255.0;
    float b = (float)(*img.data(w,h,0,2))/255.0;
    Vector3f color{r,g,b};

    return color;
}