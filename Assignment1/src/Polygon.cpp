#include <iostream>
#include <optional>
#include "DS.h"
#include "Models.h"
#include "defs.h"

Polygon::Polygon(const std::vector<Point>& points, const Material& mat, const Transformation& t)
    : Model{mat,t}, _plane{NULL} {
    assert(points.size() > 2);
    _points.push_back(points[0]);
    _points.push_back(points[1]);
    _points.push_back(points[2]);
    _plane =
        new Plane(Ray(_points[0],
                      (_points[0] - _points[1]).cross(_points[0] - _points[2])),
                  mat,t);  // src as centroid
    std::vector<Point> rejected_points;
    for (int i = 3; i < points.size(); i++) {
        if (!(_plane->_isOnSurface(points[i])))
            rejected_points.push_back(points[i]);
        else
            _points.push_back(points[i]);
    }
    _points.push_back(points[0]);
    if (rejected_points.size() > 0) {
        std::cerr << "WARNING: In Polygon construction "
                  << rejected_points.size() << " rejected for not in plane!"
                  << std::endl;
    }
}

bool Polygon::_isOnSurface(const Point& p) const {
    // std::cout<<"Checking for Point: "<<p<<std::endl;

    if (!_plane || !(_plane->_isOnSurface(p))) return false;
    // std::cout<<"Point is in plane "<<(*_plane)<<std::endl;

    Point other_p = (_points[0] + _points[1]) / 2;
    if ((other_p - p).norm() <= EPSILON) {
        // choose some other point, this one is too close
        other_p = (_points[1] + _points[2]) / 2;
    }

    Vector3f d = (p - other_p).normalized();
    Vector3f n = (d.cross((_plane->_getNormal(p)).value().dir)).normalized();

    // std::cout<<"Other_p "<<(other_p)<<std::endl;
    // std::cout<<"d "<<(d)<<std::endl;
    // std::cout<<"n "<<(n)<<std::endl;

    int num_intersections = 0;
    for (int i = 0; i < _points.size() - 1; i++) {
        Point x = _points[i];
        Point y = _points[i + 1];

        // std::cout<<"--------------------------"<<std::endl;
        // std::cout<<"x "<<(x)<<std::endl;
        // std::cout<<"y "<<(y)<<std::endl;

        float num = (p - y).dot(n);
        float denom = (x - y).dot(n);
        // if(fabs(denom)<=EPSILON) continue; // ray is almost parallel // TODO:
        // CHECK THIS OKAY?
        float t = num / denom;

        if (t <= 0 || t >= 1) continue;

        // std::cout<<"num "<<(num)<<std::endl;
        // std::cout<<"denom "<<(denom)<<std::endl;
        // std::cout<<"t "<<(t)<<std::endl;

        Point intersect_point = x * t + (1 - t) * y;
        float cos_theta = (intersect_point - p).dot(d);
        // std::cout<<"intersect_point "<<(intersect_point)<<std::endl;
        // std::cout<<"cos_theta "<<(cos_theta)<<std::endl;

        if (cos_theta < 0) continue;  // opposite to ray direction

        num_intersections++;
    }
    // std::cout<<"num_intersections "<<(num_intersections)<<std::endl;

    return ((num_intersections % 2) == 1);
}

std::optional<std::pair<float, const Model*>>
Polygon::_getIntersectionLengthAndPart(const Ray& r) const {
    if (!_plane) return {};
    auto intersection_part = _plane->_getIntersectionLengthAndPart(r);
    if (!intersection_part) return {};
    Point interp = r.src + intersection_part.value().first * r.dir;
    if (_isOnSurface(interp)) return intersection_part;
    return {};
}

std::optional<Ray> Polygon::_getNormal(const Point& p) const {
    if (!_isOnSurface(p)) return {};
    return _plane->_getNormal(p);
}

std::ostream& Polygon::print(std::ostream& os) const {
    os << "Polygon{NumPoints=" << _points.size() << ",plane:";
    if (!_plane) {
        return os << "NULL}";
    }
    return os << (*_plane) << "}";
}