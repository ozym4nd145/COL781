#pragma once

#include <Eigen/Dense>
#include <cassert>
#include <cmath>
#include <iostream>
#include <optional>
#include "Camera.h"
#include "DS.h"
#include "defs.h"

class Model {
   public:
    const Material mat;

    Model(const Material& mat) : mat{mat} {}
    virtual ~Model() = default;

    std::optional<float> getRefractiveIndex(Vector3f incident,
                                            Vector3f normal) const;
    // This function return the intersection length and a pointer to the base
    // part for getting normal efficiently
    virtual std::optional<std::pair<float, const Model*>>
    getIntersectionLengthAndPart(const Ray&) const = 0;

    virtual std::optional<Ray> getNormal(const Point&) const = 0;

    virtual bool isOnSurface(const Point&) const = 0;

    virtual std::ostream& print(std::ostream& os) const = 0;

    friend std::ostream& operator<<(std::ostream& os, const Model& m) {
        return m.print(os);
    }

    /**
     * @param{normal} Normal ray with source at the point of contact
     * @param{incident} Incident ray with direction pointing towards the point
     * of contact
     * @return {Ray} Reflected ray pointing away from point of contact with
     * source at point of contact
     */
    static Ray getReflected(const Ray& incident, const Ray& normal);

    /**
     * @param{normal} Normal ray with source at the point of contact
     * @param{incident} Incident ray with direction pointing towards the point
     * of contact
     * @return {Ray} Reflected ray pointing away from point of contact with
     * source at point of contact
     */
    static Ray getRefracted(const Ray& incident, const Ray& normal,
                            const float incident_ref_idx,
                            const float transfer_ref_idx);
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
        const Color* refracted) const;
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

    std::optional<std::pair<float, const Model*>> getIntersectionLengthAndPart(
        const Ray& r) const;
    bool isOnSurface(const Point& p) const;
    // returns outward normal
    std::optional<Ray> getNormal(const Point& p) const;
    std::ostream& print(std::ostream& os) const;
};

class Plane : public Model {
   private:
    const Ray _normal;

   public:
    Plane(const Ray& normal, Material mat) : Model{mat}, _normal{normal} {}

    std::optional<std::pair<float, const Model*>> getIntersectionLengthAndPart(
        const Ray& r) const;

    bool isOnSurface(const Point& p) const;
    std::optional<Ray> getNormal(const Point& p) const;

    std::ostream& print(std::ostream& os) const;
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
    bool isOnSurface(const Point& p) const;
    std::optional<std::pair<float, const Model*>> getIntersectionLengthAndPart(
        const Ray& r) const;
    std::optional<Ray> getNormal(const Point& p) const;
    std::ostream& print(std::ostream& os) const;
};

class Collection : public Model {
    // TODO: CHANGE TO UNIQUE_PTR
   private:
    std::vector<const Model*> _parts;
    std::optional<const Model*> getWhichPart(const Point& p) const;

   public:
    Collection(Material mat) : Model{mat} {}

    void addModel(Model* part) { _parts.push_back(part); }

    std::optional<std::pair<float, const Model*>> getIntersectionLengthAndPart(
        const Ray& r) const;

    bool isOnSurface(const Point& p) const;

    std::optional<Ray> getNormal(const Point& p) const;

    std::ostream& print(std::ostream& os) const;
};

class Quadric : public Model {
   private:
    const QuadricParams _qp;
    Matrix4f M;

   public:
    using Model::Model;
    Quadric(const QuadricParams& qp, const Material& mp) : _qp(qp), Model(mp) {
        M << qp.A, qp.B, qp.C, qp.D, qp.B, qp.E, qp.F, qp.G, qp.C, qp.F, qp.H,
            qp.I, qp.D, qp.G, qp.I, qp.J;
    }

    std::optional<std::pair<float, const Model*>> getIntersectionLengthAndPart(
        const Ray& r) const;
    bool isOnSurface(const Point& p) const;
    std::optional<Ray> getNormal(const Point& p) const;

    std::ostream& print(std::ostream& os) const;
};

class Box : public Model {
   private:
    const Point _center;
    const float _l;
    const float _b;
    const float _h;
    const Vector3f _ax;
    const Vector3f _az;
    const Vector3f _ay;
    Collection _coll;

   public:
    Box(const Point& center, const Vector3f& x, const Vector3f& y, float l,
        float b, float h, const Material& mat);
    std::optional<std::pair<float, const Model*>> getIntersectionLengthAndPart(
        const Ray& r) const;
    bool isOnSurface(const Point& p) const;
    std::optional<Ray> getNormal(const Point& p) const;
    std::ostream& print(std::ostream& os) const;
};

struct State {
    std::vector<Model*> models;
    std::vector<Light*> lights;
    std::unordered_map<std::string, Material*> materials;
    Camera* cam;
};

class Polygon : public Model {
   private:
    std::vector<Point> _points;
    Plane* _plane;

   public:
    // assume that points are given in counter clockwise order and outward
    // normal is given by right hand curl rule
    Polygon(const std::vector<Point>& points, const Material& mat);
    bool isOnSurface(const Point& p) const;
    std::optional<std::pair<float, const Model*>> getIntersectionLengthAndPart(
        const Ray& r) const;
    std::optional<Ray> getNormal(const Point& p) const;
    std::ostream& print(std::ostream& os) const;
};