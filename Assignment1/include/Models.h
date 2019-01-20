#pragma once

#include <optional>
#include <iostream>
#include "DS.h"

class Model {
    typedef Triple<float> Vec;
    public:
        virtual ~Model() = default;
        virtual std::optional<float> getIntersectionLength(const Ray&) const = 0;
        virtual std::optional<Ray> getNormal(const Point&) const = 0; // returns outward normal
        virtual Color getColor() const = 0; // returns color
        static Ray getReflected(const Ray& incident, const Ray& normal) {
            const float angle = (normal.dir).dot(incident.dir);
            const bool isInside = angle > 0;
            const float abs_angle = fabs(angle);
            Vec true_normal = isInside?(-normal.dir):(normal.dir);
            Vec reflected_dir = (2*abs_angle*true_normal) + incident.dir;
            return Ray(normal.src,reflected_dir);
        }
};

class Sphere: public Model {
    private:
        const Point _center;
        const float _radius;
        const float _radius_sq;
        const Color _color;
    public:
        Sphere(Point center, float radius, Color c): _center{center}, _radius{radius}, _radius_sq{_radius*_radius}, _color{c}
        {}
        Sphere(const Sphere& s): _center{s._center},_radius{s._radius},_radius_sq{s._radius_sq},_color{s._color} {}
        Sphere& operator=(const Sphere& s) = delete;
        
        Color getColor() const {
            return _color;
        }

        std::optional<float> getIntersectionLength(const Ray& r) const {
            // TODO: check intersection inside Sphere
            Ray src_center(r.src,_center-r.src);
            const float cos_theta = src_center.dir.dot(r.dir);
            if(src_center.length > _radius && cos_theta <= 0) {
                return {}; // ray can't intersect
            }

            const float src_center_sq = src_center.length*src_center.length;
            const float src_center_proj = src_center.length*cos_theta;
            const float center_ray_sq = src_center_sq - src_center_proj*src_center_proj;

            if(center_ray_sq > _radius_sq) return {}; // not intersecting
            const float d = sqrt(_radius_sq-center_ray_sq);
            const float t0 = src_center_proj+d;
            const float t1 = src_center_proj-d;

            float min_dist = std::min(t0,t1);
            float max_dist = std::max(t0,t1);
            float dist = (min_dist < 0)?max_dist:min_dist; // use correct distance
            return dist;
        }

        std::optional<Ray> getNormal(const Point& p) const { // returns outward normal
            return Ray(p,p-_center);
        }

        friend std::ostream& operator<<(std::ostream& os, const Sphere& sp) {
            return os<<"Sphere{center="<<sp._center<<",radius="<<sp._radius<<"}";
        }
};