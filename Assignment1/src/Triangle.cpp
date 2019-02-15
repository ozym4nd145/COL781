#include <iostream>
#include <optional>
#include "DS.h"
#include "Models.h"
#include "defs.h"
#include "utils.h"


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

std::optional<Color> Triangle::getTexture(const Point& p) const {
    if((this->mat).img == NULL) return {};

    float a3 = getArea(_p1, _p2, p)/_area;
    float a2 = getArea(_p1, _p3, p)/_area;
    float a1 = getArea(_p2, _p3, p)/_area;
    float sum = a1+a2+a3;
    a2 /= sum;
    a3 /= sum;

    float u = a2;
    float v = a3;

    auto& img = *((this->mat).img);

    int w = u*img.width();
    int h = v*img.height();

    float r = (float)(*img.data(w,h,0,0))/255.0;
    float g = (float)(*img.data(w,h,0,1))/255.0;
    float b = (float)(*img.data(w,h,0,2))/255.0;
    Vector3f color{r,g,b};

    return color;
}