#include <iostream>
#include <optional>
#include "DS.h"
#include "Models.h"
#include "defs.h"

std::optional<std::pair<float, const Model*>> Plane::_getIntersectionLengthAndPart(const Ray& r) const {
    float cos_theta = r.dir.dot(_normal.dir);
    if (fabs(cos_theta) <= EPSILON) return {};  // not intersecting case
    float t = ((_normal.src - r.src).dot(_normal.dir)) / cos_theta;
    if (t < 0) return {};
    return std::make_pair(t, this);
}

bool Plane::_isOnSurface(const Point& p) const {
    return (fabs((_normal.src - p).dot(_normal.dir)) <= EPSILON);
}

std::optional<Ray> Plane::_getNormal(const Point& p) const {
    if (!_isOnSurface(p)) return {};
    return Ray(p, _normal.dir);
};

std::ostream& Plane::print(std::ostream& os) const {
    return os << "Plane{normal=" << _normal << "}";
}
