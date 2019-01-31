#include <iostream>
#include <optional>
#include "DS.h"
#include "Models.h"
#include "defs.h"

bool Triangle::isOnSurface(const Point& p) const {
    float a1 = getArea(_p1, _p2, p);
    float a2 = getArea(_p1, _p3, p);
    float a3 = getArea(_p2, _p3, p);
    return (fabs(a1 + a2 + a3 - _area) <= EPSILON);
}

std::optional<std::pair<float, const Model*>>
Triangle::getIntersectionLengthAndPart(const Ray& r) const {
    auto intersection_part = _plane.getIntersectionLengthAndPart(r);
    if (!intersection_part) return {};
    Point interp = r.src + intersection_part.value().first * r.dir;
    if (isOnSurface(interp)) return intersection_part;
    return {};
}

std::optional<Ray> Triangle::getNormal(const Point& p) const {
    if (!isOnSurface(p)) return {};
    return _plane.getNormal(p);
};

std::ostream& Triangle::print(std::ostream& os) const {
    return os << "Triangle{p1=" << _p1 << ",p2=" << _p2 << ",p3=" << _p3 << "}";
}
