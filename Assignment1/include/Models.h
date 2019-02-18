#pragma once

#include <Eigen/Dense>
#include <cassert>
#include <cmath>
#include <iostream>
#include <optional>
#include "Camera.h"
#include "DS.h"
#include "defs.h"
#include "OGLModels.h"

class Model {
   public:
    std::optional<float> _getRefractiveIndex(Vector3f incident,
                                             Vector3f normal) const;
    // This function return the intersection length and a pointer to the base
    // part for getting normal efficiently
    virtual std::optional<std::pair<float, const Model*>>
    _getIntersectionLengthAndPart(const Ray&) const = 0;
    virtual std::optional<Ray> _getNormal(const Point&) const = 0;
    virtual bool _isOnSurface(const Point&) const = 0;

    /**
     * @param{normal} Normal ray with source at the point of contact
     * @param{incident} Incident ray with direction pointing towards the point
     * of contact
     * @return {Ray} Reflected ray pointing away from point of contact with
     * source at point of contact
     */
    Ray _getReflected(const Ray& incident, const Ray& normal) const;

    /**
     * @param{normal} Normal ray with source at the point of contact
     * @param{incident} Incident ray with direction pointing towards the point
     * of contact
     * @return {Ray} Reflected ray pointing away from point of contact with
     * source at point of contact
     */
    Ray _getRefracted(const Ray& incident, const Ray& normal,
                             const float incident_ref_idx,
                             const float transfer_ref_idx) const;
    /**
     * @param{normal} Normal direction at the point of color
     * @param{view} View direction pointing away from the point of color
     * @param{lights} pair of {intensity,dir} for light sourcer where dir is
     * pointing away from point of color
     * @param{ambient} ambient intensity in the room
     * @param{reflected} reflected intensity at the point of color
     * @param{refracted} refracted intensity at the point of color
     */
    virtual Color _getIntensity(
        const Vector3f& normal, const Vector3f& view,
        const std::vector<std::pair<Color, Vector3f>>& lights,
        const Color* ambient, const Color* reflected,
        const Color* refracted, std::optional<Color> texture) const;
    
    /**
     * @param{intensity} Intensity of illumination at the point of interest
     * @param{p} point of interest for which texture value is required
     */
    virtual std::optional<Color> _getTexture(const Point& p) const;

    const Material mat;
    Transformation trans;

    Model(const Material& mat, const Transformation& trans)
        : mat{mat}, trans(trans) {}
    virtual ~Model() = default;

    std::optional<float> getRefractiveIndex(Vector3f incident,
                                            Vector3f normal) const;
    std::optional<std::pair<float, const Model*>> getIntersectionLengthAndPart(
        const Ray&) const;
    std::optional<Ray> getNormal(const Point&) const;
    bool isOnSurface(const Point&) const;

    Ray getReflected(const Ray& incident, const Ray& normal) const;
    Ray getRefracted(const Ray& incident, const Ray& normal,
                            const float incident_ref_idx,
                            const float transfer_ref_idx) const;
    Color getIntensity(const Vector3f& normal, const Vector3f& view,
                       const std::vector<std::pair<Color, Vector3f>>& lights,
                       const Color* ambient, const Color* reflected,
                       const Color* refracted, std::optional<Color> texture) const;
    std::optional<Color> getTexture(const Point& p) const;

    virtual std::ostream& print(std::ostream& os) const = 0;
    friend std::ostream& operator<<(std::ostream& os, const Model& m) {
        return m.print(os);
    }
};

class Light {
    Point _center;
    Color _intensity;

   public:
    Light(Point center, Color intensity)
        : _center{center}, _intensity{intensity} {}
    Ray getRayToLight(Point p) const { return Ray(p, _center - p); }
    Color getIntensity() const { return _intensity; }
    Point getCenter() const {return _center;}
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
    std::optional<std::pair<float, const Model*>> _getIntersectionLengthAndPart(
        const Ray& r) const;
    bool _isOnSurface(const Point& p) const;
    // returns outward normal
    std::optional<Ray> _getNormal(const Point& p) const;

    Sphere(Point center, float radius, Material mat, Transformation t)
        : Model{mat, t},
          _center{center},
          _radius{radius},
          _radius_sq{_radius * _radius} {}
    Sphere(const Sphere& s)
        : Model{s.mat, s.trans},
          _center{s._center},
          _radius{s._radius},
          _radius_sq{s._radius_sq} {}
    Sphere& operator=(const Sphere& s) = delete;
    std::ostream& print(std::ostream& os) const;

    std::optional<Color> _getTexture(const Point& p) const override;
};

class Plane : public Model {
   private:
    const Ray _normal;

   public:
    std::optional<std::pair<float, const Model*>> _getIntersectionLengthAndPart(
        const Ray& r) const;
    bool _isOnSurface(const Point& p) const;
    std::optional<Ray> _getNormal(const Point& p) const;

    Plane(const Ray& normal, Material mat, Transformation t)
        : Model{mat, t}, _normal{normal} {}
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
    bool _isOnSurface(const Point& p) const;
    std::optional<std::pair<float, const Model*>> _getIntersectionLengthAndPart(
        const Ray& r) const;
    std::optional<Ray> _getNormal(const Point& p) const;

    Triangle(const Point& p1, const Point& p2, const Point& p3,
             const Material& mat, const Transformation& t)
        : Model{mat, t},
          _p1{p1},
          _p2{p2},
          _p3{p3},
          _plane{Ray(_p1, ((_p1 - _p2).cross(_p1 - _p3))), mat, t},
          _area{getArea(_p1, _p2, _p3)} {}
    std::ostream& print(std::ostream& os) const;
    std::optional<Color> _getTexture(const Point& p) const override;
};

class Collection : public Model {
    // TODO: CHANGE TO UNIQUE_PTR
   private:
    std::vector<const Model*> _parts;
    std::optional<const Model*> getWhichPart(const Point& p) const;

   public:
    std::optional<std::pair<float, const Model*>> _getIntersectionLengthAndPart(
        const Ray& r) const;
    bool _isOnSurface(const Point& p) const;
    std::optional<Ray> _getNormal(const Point& p) const;

    Collection(Material mat, Transformation t) : Model{mat, t} {}

    void addModel(Model* part) { 
        part->trans = this->trans;
        _parts.push_back(part); 
    }

    std::ostream& print(std::ostream& os) const;
};

class Quadric : public Model {
   private:
    const QuadricParams _qp;
    Matrix4f M;

   public:
    std::optional<std::pair<float, const Model*>> _getIntersectionLengthAndPart(
        const Ray& r) const;
    bool _isOnSurface(const Point& p) const;
    std::optional<Ray> _getNormal(const Point& p) const;

    using Model::Model;
    Quadric(const QuadricParams& qp, const Material& mp,
            const Transformation& t)
        : _qp(qp), Model(mp, t) {
        M << qp.A, qp.B, qp.C, qp.D, qp.B, qp.E, qp.F, qp.G, qp.C, qp.F, qp.H,
            qp.I, qp.D, qp.G, qp.I, qp.J;
    }

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
    std::optional<std::pair<float, const Model*>> _getIntersectionLengthAndPart(
        const Ray& r) const;
    bool _isOnSurface(const Point& p) const;
    std::optional<Ray> _getNormal(const Point& p) const;

    Box(const Point& center, const Vector3f& x, const Vector3f& y, float l,
        float b, float h, const Material& mat, const Transformation& t);
    std::ostream& print(std::ostream& os) const;
};


class Polygon : public Model {
   private:
    std::vector<Point> _points;
    Plane* _plane;

   public:
    bool _isOnSurface(const Point& p) const;
    std::optional<std::pair<float, const Model*>> _getIntersectionLengthAndPart(
        const Ray& r) const;
    std::optional<Ray> _getNormal(const Point& p) const;

    // assume that points are given in counter clockwise order and outward
    // normal is given by right hand curl rule
    Polygon(const std::vector<Point>& points, const Material& mat,
            const Transformation& t);
    std::ostream& print(std::ostream& os) const;
};

class Background {
    private:
        Sphere* world;
        Color background;
        bool has_background;
    public:
        Background(Color c): world(NULL),background(c),has_background(false) {}
        Background(const std::string& img_path): world(NULL),background(Vector3f::Zero()),has_background(true) {
            Material background_material;
            background_material.setTexture(img_path);
            world = new Sphere(Vector3f::Zero(),1,background_material,IDENTITY_TRANS);
        }
        Color getTexture(const Ray& r) const {
            if(!has_background) return background;
            auto texture = world->getTexture(r.dir);
            if(!(texture)) {
                std::cerr<<"WARNING: Background did not return any texture for ray: "<<r<<" with sphere as: "<<world<<std::endl;
                return background;
            }
            return texture.value();
        }
};


struct State {
    std::vector<Model*> models;
    std::vector<ogl::BaseModel *> oglModels;
    std::vector<Light*> lights;
    std::unordered_map<std::string, Material*> materials;
    Camera* cam;
    Background* bg;
    std::pair<int,int> tracePoint;
};