#pragma once

#include <Eigen/Dense>
#include <cassert>
#include <cmath>
#include <iostream>
#include <optional>
#include "DS.h"

class Model {
   public:
    const Material mat;

    Model(const Material& mat) : mat{mat} {}
    virtual ~Model() = default;

    std::optional<float> getRefractiveIndex(Vector3f incident,
                                            Vector3f normal) const {
        if ((this->mat).refractive_index < 0) return {};
        // if hit from inside the return refractive index as 1 for the outside
        // world
        return ((incident.dot(normal) < 0) ? ((this->mat).refractive_index)
                                           : 1.0);
    }

    virtual std::optional<float> getIntersectionLength(const Ray&) const = 0;

    // returns outward normal
    virtual std::optional<Ray> getNormal(const Point&) const = 0;

    /**
     * @param{normal} Normal ray with source at the point of contact
     * @param{incident} Incident ray with direction pointing towards the point
     * of contact
     * @return {Ray} Reflected ray pointing away from point of contact with
     * source at point of contact
     */
    static Ray getReflected(const Ray& incident, const Ray& normal) {
        const float angle = (normal.dir).dot(incident.dir);
        const bool isInside = angle > 0;
        const float abs_angle = fabs(angle);
        Vector3f true_normal = isInside ? (-normal.dir) : (normal.dir);
        Vector3f reflected_dir = (2 * abs_angle * true_normal) + incident.dir;

        return Ray(normal.src + (true_normal * EPSILON),
                   reflected_dir);  // Made the starting point slightly outside
                                    // along reflection dir
    }

    /**
     * @param{normal} Normal ray with source at the point of contact
     * @param{incident} Incident ray with direction pointing towards the point
     * of contact
     * @return {Ray} Reflected ray pointing away from point of contact with
     * source at point of contact
     */
    static Ray getRefracted(const Ray& incident, const Ray& normal,
                            const float incident_ref_idx,
                            const float transfer_ref_idx) {
        const float cos_theta_i = (normal.dir).dot(incident.dir);
        const bool isInside = cos_theta_i > 0;
        const float abs_cos_theta_i = fabs(cos_theta_i);
        const float abs_sin_theta_i =
            sqrt(1 - (abs_cos_theta_i * abs_cos_theta_i));
        const float ref_ratio = incident_ref_idx / transfer_ref_idx;
        const float abs_sin_theta_t = (ref_ratio * abs_sin_theta_i);
        const float abs_cos_theta_t =
            sqrt(1 - (abs_sin_theta_t * abs_sin_theta_t));

        Vector3f true_normal = isInside ? (-normal.dir) : (normal.dir);

        Vector3f refracted_dir =
            ((ref_ratio * (incident.dir + true_normal * abs_cos_theta_i)) -
             (abs_cos_theta_t * true_normal))
                .normalized();

        // std::cout<<"Normal src: "<<normal.src<<"RefractedDir:
        // "<<refracted_dir<<" delta: "<<(refracted_dir*EPSILON)<<std::endl;
        return Ray(normal.src - (true_normal * EPSILON),
                   refracted_dir);  // Made the starting point slightly outside
                                    // along refraction dir
    }

    /**
     * @param{normal} Normal direction at the point of color
     * @param{view} View direction pointing away from the point of color
     * @param{lights} pair of {intensity,dir} for light sourcer where dir is
     * pointing away from point of color
     * @param{ambient} ambient intensity in the room
     * @param{reflected} reflected intensity at the point of color
     * @param{refracted} refracted intensity at the point of color
     */
    virtual Color getIntensity(
        const Vector3f& normal, const Vector3f& view,
        const std::vector<std::pair<Color, Vector3f>>& lights,
        const Color* ambient, const Color* reflected,
        const Color* refracted) const {
        Vector3f view_corr = view;
        Vector3f normal_corr = normal;
        if (view_corr.dot(normal_corr) < 0) {
            // std::cout<<"NORMAL AND VIEW OPPOSITE!!"<<std::endl;

            // we are vieweing wrong normal (maybe the ray in inside the
            // object?)
            normal_corr = -normal_corr;
        }

        Color final_color(0, 0, 0);

        // assert(lights.size()==1);

        for (const auto pr : lights) {
            Color intensity = pr.first;
            Vector3f incident = pr.second;
            float cos_theta = incident.dot(normal_corr);
            if (cos_theta < 0) {
                // light strikes from opposite side
                continue;
            }
            Vector3f reflected = 2 * (cos_theta)*normal_corr - incident;
            // some duplicate code with getReflected function
            final_color += ((this->mat).Kd).cwiseProduct(intensity) * cos_theta;
            float cos_alpha = reflected.dot(view_corr);
            if (cos_alpha > 0) {
                final_color += ((this->mat).Ks).cwiseProduct(intensity) *
                               pow(cos_alpha, (this->mat).specular_coeff);
            }
        }

        if (ambient) {
            final_color += ((this->mat).Ka).cwiseProduct(*ambient);
        }

        if (reflected) {
            final_color += ((this->mat).Krg).cwiseProduct(*reflected);
        }

        if (refracted) {
            final_color += ((this->mat).Ktg).cwiseProduct(*refracted);
        }

        return final_color;
    }
};

class Light {
   private:
    Point _center;
    Color _intensity;

   public:
    Light(Point center, Color intensity)
        : _center{center}, _intensity{intensity} {}
    Ray getRayToLight(Point p) const { return Ray(p, _center - p); }
    Color getIntensity() const { return _intensity; }
    friend std::ostream& operator<<(std::ostream& os, const Light& lg) {
        return os << "Light{center=" << lg._center
                  << ",intensity=" << lg._intensity << "}";
    }
};

class Sphere : public Model {
   private:
    const Point _center;
    const float _radius;
    const float _radius_sq;

   public:
    Sphere(Point center, float radius, Material mat)
        : Model{mat},
          _center{center},
          _radius{radius},
          _radius_sq{_radius * _radius} {}
    Sphere(const Sphere& s)
        : Model{s.mat},
          _center{s._center},
          _radius{s._radius},
          _radius_sq{s._radius_sq} {}
    Sphere& operator=(const Sphere& s) = delete;

    std::optional<float> getIntersectionLength(const Ray& r) const {
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
        float dist =
            (min_dist < 0) ? max_dist : min_dist;  // use correct distance
        return dist;
    }

    std::optional<Ray> getNormal(
        const Point& p) const {  // returns outward normal
        return Ray(p, p - _center);
    }

    friend std::ostream& operator<<(std::ostream& os, const Sphere& sp) {
        return os << "Sphere{center=" << sp._center << ",radius=" << sp._radius
                  << "}";
    }
};

class Plane : public Model {
   private:
    const Ray _normal;

   public:
    Plane(const Ray& normal, Material mat) : Model{mat}, _normal{normal} {}
    std::optional<float> getIntersectionLength(const Ray& r) const {
        float cos_theta = r.dir.dot(_normal.dir);
        if (fabs(cos_theta) <= EPSILON) return {};  // not intersecting case
        float t = ((_normal.src - r.src).dot(_normal.dir)) / cos_theta;
        if (t < 0) return {};
        return t;
    }
    std::optional<Ray> getNormal(
        const Point& p) const {  // returns outward normal
        // TODO: check if point on the plane
        return _normal;
    }
    friend std::ostream& operator<<(std::ostream& os, const Plane& pl) {
        return os << "Plane{normal=" << pl._normal << "}";
    }
};

class Triangle : public Model {
   private:
    const Point _p1;
    const Point _p2;
    const Point _p3;
    const Plane _plane;
    const float _area;

    static float getArea(Point a, Point b, Point c) {
        return fabs((a - b).cross(a - c).norm() / 2);
    }

   public:
    Triangle(const Point& p1, const Point& p2, const Point& p3,
             const Material& mat)
        : Model{mat},
          _p1{p1},
          _p2{p2},
          _p3{p3},
          _plane{Ray(_p1, ((_p1 - _p2).cross(_p1 - _p3))), mat},
          _area{getArea(_p1, _p2, _p3)} {}
    std::optional<float> getIntersectionLength(const Ray& r) const {
        auto intersection_len = _plane.getIntersectionLength(r);
        if (!intersection_len) return {};
        Point interp = r.src + intersection_len.value() * r.dir;
        float a1 = getArea(_p1, _p2, interp);
        float a2 = getArea(_p1, _p3, interp);
        float a3 = getArea(_p2, _p3, interp);
        if (fabs(a1 + a2 + a3 - _area) <= EPSILON) return intersection_len;
        return {};
    }
    std::optional<Ray> getNormal(
        const Point& p) const {  // returns outward normal
        return _plane.getNormal(p);
    }
    friend std::ostream& operator<<(std::ostream& os, const Triangle& tr) {
        return os << "Triangle{p1=" << tr._p1 << ",p2=" << tr._p2
                  << ",p3=" << tr._p3 << "}";
    }
};

class Quadric : public Model {
    const QuadricParams _qp;
    Matrix4f M;

   public:
    using Model::Model;
    Quadric(const QuadricParams& qp, const Material& mp) : _qp(qp), Model(mp) {
        M << qp.A, qp.B, qp.C, qp.D, qp.B, qp.E, qp.F, qp.G, qp.C, qp.F, qp.H,
            qp.I, qp.D, qp.G, qp.I, qp.J;
    }

    std::optional<float> getIntersectionLength(const Ray& r) const {
        auto Ro = augment(r.src, 1.0);
        auto Rd = augment(r.dir, 0.0);
        float Aq = Rd * M * (Rd.transpose());
        float Bq = Rd * M * (Ro.transpose());
        Bq += Ro * M * (Rd.transpose());
        float Cq = Ro * M * (Ro.transpose());
        auto inters = solve_quadratic(Aq, Bq, Cq);
        if (!inters) return {};
        auto min_pos_dist = (inters.value().first < 0) ? (inters.value().second)
                                                       : (inters.value().first);
        if (min_pos_dist < 0) return {};
        return min_pos_dist;
    }

    std::optional<Ray> getNormal(const Point& p) const {
        float val = augment(p, 1.0) * M * (augment(p, 1.0).transpose());
        if (abs(val) > 10 * EPSILON) return {};
        float nx = 2.0 * (_qp.A * p[0] + _qp.B * p[1] + _qp.C * p[2] + _qp.D);
        float ny = 2.0 * (_qp.B * p[0] + _qp.E * p[1] + _qp.F * p[2] + _qp.G);
        float nz = 2.0 * (_qp.C * p[0] + _qp.F * p[1] + _qp.H * p[2] + _qp.I);
        return Ray(p, Vector3f(nx, ny, nz));
    }
};