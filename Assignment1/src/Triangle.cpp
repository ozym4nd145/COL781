#include <iostream>
#include <optional>
#include "DS.h"
#include "Models.h"
#include "defs.h"

bool Triangle::_isOnSurface(const Point& p) const {
    float a1 = getArea(_p1, _p2, p);
    float a2 = getArea(_p1, _p3, p);
    float a3 = getArea(_p2, _p3, p);
    return (fabs(a1 + a2 + a3 - _area) <= EPSILON);
}

std::optional<std::pair<float, const Model*>>
Triangle::_getIntersectionLengthAndPart(const Ray& r) const {
    auto intersection_part = _plane._getIntersectionLengthAndPart(r);
    if (!intersection_part) return {};
    Point interp = r.src + intersection_part.value().first * r.dir;
    if (_isOnSurface(interp)) return std::make_pair(intersection_part.value().first,this);
    return {};
}

std::optional<Ray> Triangle::_getNormal(const Point& p) const {
    if (!_isOnSurface(p)) return {};
    return _plane._getNormal(p);
};

std::ostream& Triangle::print(std::ostream& os) const {
    return os << "Triangle{p1=" << _p1 << ",p2=" << _p2 << ",p3=" << _p3 << "}";
}
