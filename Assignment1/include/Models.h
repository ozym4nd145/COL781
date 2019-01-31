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

    //This function return the intersection length and a pointer to the base part for getting normal efficiently
    virtual std::optional<std::pair<float,const Model*>> getIntersectionLengthAndPart(const Ray&) const = 0;
    
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

    std::optional<std::pair<float,const Model*>> getIntersectionLengthAndPart(const Ray& r) const {
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
        return std::make_pair(dist,this);
    }

    bool isOnSurface(const Point& p) const {
        return (fabs((p-_center).norm()-_radius)<=EPSILON);
    }
    
    // returns outward normal
    std::optional<Ray> getNormal(const Point& p) const {
        if(!isOnSurface(p)) return {};
        auto normal = Ray(p, p - _center);
        return normal;
    };

    std::ostream& print(std::ostream& os) const {
        return os << "Sphere{center=" << _center << ",radius=" << _radius << "}";
    }
};

class Plane: public Model {
    private:
        const Ray _normal;
    public:
        Plane(const Ray& normal, Material mat): Model{mat}, _normal{normal} {}

        std::optional<std::pair<float,const Model*>> getIntersectionLengthAndPart(const Ray& r) const {
            float cos_theta = r.dir.dot(_normal.dir);
            if(fabs(cos_theta)<=EPSILON) return {}; // not intersecting case
            float t = ((_normal.src-r.src).dot(_normal.dir))/cos_theta;
            if(t<0) return {};
            return std::make_pair(t,this);
        }

        bool isOnSurface(const Point& p) const {
            return (fabs((_normal.src-p).dot(_normal.dir))<=EPSILON);
        }

        std::optional<Ray> getNormal(const Point& p) const {
            if(!isOnSurface(p)) return {};
            return Ray(p,_normal.dir);
        };

        std::ostream& print(std::ostream& os) const {
            return os<<"Plane{normal="<<_normal<<"}";
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
        Triangle(const Point& p1, const Point& p2, const Point& p3, const Material& mat):
        Model{mat}, _p1{p1},_p2{p2},_p3{p3},
        _plane{Ray(_p1,((_p1-_p2).cross(_p1-_p3))),mat},
        _area{getArea(_p1,_p2,_p3)}
        {}

        bool isOnSurface(const Point& p) const {
            float a1 = getArea(_p1,_p2,p);
            float a2 = getArea(_p1,_p3,p);
            float a3 = getArea(_p2,_p3,p);
            return (fabs(a1+a2+a3-_area)<=EPSILON);
        }
        
        std::optional<std::pair<float,const Model*>> getIntersectionLengthAndPart(const Ray& r) const {
            auto intersection_part = _plane.getIntersectionLengthAndPart(r);
            if(!intersection_part) return {};
            Point interp = r.src+intersection_part.value().first*r.dir;
            if(isOnSurface(interp)) return intersection_part;
            return {};
        }


        std::optional<Ray> getNormal(const Point& p) const {
            if(!isOnSurface(p)) return {};
            return _plane.getNormal(p);
        };

        std::ostream& print(std::ostream& os) const {
            return os<<"Triangle{p1="<<_p1<<",p2="<<_p2<<",p3="<<_p3<<"}";
        }
};

class Collection: public Model {
    // TODO: CHANGE TO UNIQUE_PTR
    private:
        std::vector<const Model*> _parts;

        std::optional<const Model*> getWhichPart(const Point& p) const {
            for (auto part : _parts) {
                if(part->isOnSurface(p)) return part;
            }
            return {};
        }
    
    public:
        Collection(Material mat):Model{mat} {
        }

        void addModel(Model* part) {
            _parts.push_back(part);
        }

        std::optional<std::pair<float,const Model*>> getIntersectionLengthAndPart(const Ray& r) const {
            const Model* closest_model_part = NULL;
            float closest_distance = std::numeric_limits<float>::infinity();

            for (auto part : _parts) {
                auto intersection_part = part->getIntersectionLengthAndPart(r);
                if (!intersection_part) continue;
                auto len = intersection_part.value().first;
                if (len < closest_distance) {
                    closest_distance = len;
                    closest_model_part = part;
                }
            }

            if(!closest_model_part) return {};
            return std::make_pair(closest_distance,closest_model_part);
        }


        bool isOnSurface(const Point& p) const {
            return getWhichPart(p).has_value();
        }

        std::optional<Ray> getNormal(const Point& p) const { // returns outward normal
            auto part_opt = getWhichPart(p);
            if(!part_opt) return {};
            auto part = part_opt.value();
            return part->getNormal(p);
        }

        std::ostream& print(std::ostream& os) const {
            return os<<"Collection{num_parts="<<_parts.size()<<"}";
        }
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

    std::optional<std::pair<float,const Model*>> getIntersectionLengthAndPart(const Ray& r) const {
        auto Ro = augment(r.src, 1.0);
        auto Rd = augment(r.dir, 0.0);
        float Aq = Rd * M * (Rd.transpose());
        float Bq = Rd * M * (Ro.transpose());
        Bq += Ro * M * (Rd.transpose());
        float Cq = Ro * M * (Ro.transpose());
        auto inters = solve_quadratic(Aq, Bq, Cq);
        if (!inters) return {};
        auto min_pos_dist = (inters.value().first < 0)?(inters.value().second):(inters.value().first);
        if(min_pos_dist<0) return {};
        return std::make_pair(min_pos_dist,this);
    }
    
    bool isOnSurface(const Point& p) const {
        float val = augment(p, 1.0) * M * (augment(p, 1.0).transpose());
        return (abs(val) <= 10*EPSILON);
    }

    std::optional<Ray> getNormal(const Point& p) const {
        if(!isOnSurface(p)) return {};
        float nx = 2.0 * (_qp.A * p[0] + _qp.B * p[1] + _qp.C * p[2] + _qp.D);
        float ny = 2.0 * (_qp.B * p[0] + _qp.E * p[1] + _qp.F * p[2] + _qp.G);
        float nz = 2.0 * (_qp.C * p[0] + _qp.F * p[1] + _qp.H * p[2] + _qp.I);
        return Ray(p, Vector3f(nx, ny, nz));
    }

    std::ostream& print(std::ostream& os) const {
        return os<<"Quadric{Matrix="<<std::endl<<M<<std::endl<<"}";
    }
};

class Box: public Model {
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
        Box(const Point& center, const Vector3f& x, const Vector3f& y, float l, float b, float h, const Material& mat):
        Model{mat}, _center{center}, _l{l}, _b{b}, _h{h}, _ax{x.normalized()},
        _az{(_ax.cross(y)).normalized()}, _ay{(_az.cross(_ax)).normalized()},_coll{mat}
        {
            float l2 = _l/2;
            float b2 = _b/2;
            float h2 = _h/2;
            Point c = _center;
            Point ubl = c + _az*b2 + _ay*h2 - _ax*l2; // upper bottom left
            Point ubr = c + _az*b2 + _ay*h2 + _ax*l2; // upper bottom right
            Point utl = c - _az*b2 + _ay*h2 - _ax*l2; // upper top left
            Point utr = c - _az*b2 + _ay*h2 + _ax*l2; // upper top right
            Point lbl = c + _az*b2 - _ay*h2 - _ax*l2; // lower bottom left
            Point lbr = c + _az*b2 - _ay*h2 + _ax*l2; // lower bottom right
            Point ltl = c - _az*b2 - _ay*h2 - _ax*l2; // lower top left
            Point ltr = c - _az*b2 - _ay*h2 + _ax*l2; // lower top right

            // top face
            _coll.addModel(new Triangle(utl,utr,ubr,mat));
            _coll.addModel(new Triangle(utl,ubl,ubr,mat));
            // bottom face
            _coll.addModel(new Triangle(ltl,ltr,lbr,mat));
            _coll.addModel(new Triangle(ltl,lbl,lbr,mat));
            // front face
            _coll.addModel(new Triangle(ubl,ubr,lbr,mat));
            _coll.addModel(new Triangle(ubl,lbl,lbr,mat));
            // back face
            _coll.addModel(new Triangle(utl,utr,ltr,mat));
            _coll.addModel(new Triangle(utl,ltl,lbr,mat));
            // left face
            _coll.addModel(new Triangle(ubl,utl,ltl,mat));
            _coll.addModel(new Triangle(ubl,lbl,ltl,mat));
            // right face
            _coll.addModel(new Triangle(ubr,utr,ltr,mat));
            _coll.addModel(new Triangle(ubr,lbr,ltr,mat));
        }

        std::optional<std::pair<float,const Model*>> getIntersectionLengthAndPart(const Ray& r) const {
            return _coll.getIntersectionLengthAndPart(r);
        }

        bool isOnSurface(const Point& p) const {
            return _coll.isOnSurface(p);
        }

        std::optional<Ray> getNormal(const Point& p) const {
            return _coll.getNormal(p);
        }
     
        std::ostream& print(std::ostream& os) const {
           return os<<"Box{l="<<_l<<",b="<<_b<<",h="<<_h<<",ax="<<_ax<<",ay="<<_ay<<",az="<<_az<<"}";
        }
};

class Polygon: public Model {
    private:
        std::vector<Point> _points;
        Plane* _plane;

    public:
        // assume that points are given in counter clockwise order and outward normal is given by right hand curl rule
        Polygon(const std::vector<Point>& points,const Material& mat):Model{mat},_plane{NULL} {
            assert(points.size()>2);
            _points.push_back(points[0]);
            _points.push_back(points[1]);
            _points.push_back(points[2]);
            _plane = new Plane(Ray(_points[0],(_points[0]-_points[1]).cross(_points[0]-_points[2])),mat); // src as centroid
            std::vector<Point> rejected_points;
            for(int i=3;i<points.size();i++) {
                if(!(_plane->isOnSurface(points[i]))) rejected_points.push_back(points[i]);
                else _points.push_back(points[i]);
            }
            _points.push_back(points[0]);
            if(rejected_points.size()>0) {
                std::cerr<<"WARNING: In Polygon construction "<<rejected_points.size()<<" rejected for not in plane!"<<std::endl;
            }
        }

        bool isOnSurface(const Point& p) const {
            // std::cout<<"Checking for Point: "<<p<<std::endl;

            if(!_plane || !(_plane->isOnSurface(p))) return false;
            // std::cout<<"Point is in plane "<<(*_plane)<<std::endl;

            Point other_p=(_points[0]+_points[1])/2;
            if((other_p-p).norm()<=EPSILON) {
                // choose some other point, this one is too close
                other_p = (_points[1]+_points[2])/2;
            }
            


            Vector3f d = (p-other_p).normalized();
            Vector3f n = (d.cross((_plane->getNormal(p)).value().dir)).normalized();

            // std::cout<<"Other_p "<<(other_p)<<std::endl;
            // std::cout<<"d "<<(d)<<std::endl;
            // std::cout<<"n "<<(n)<<std::endl;

            int num_intersections = 0;
            for(int i=0;i<_points.size()-1;i++) {

                Point x = _points[i];
                Point y = _points[i+1];

                // std::cout<<"--------------------------"<<std::endl;
                // std::cout<<"x "<<(x)<<std::endl;
                // std::cout<<"y "<<(y)<<std::endl;

                float num = (p-y).dot(n);
                float denom = (x-y).dot(n);
                // if(fabs(denom)<=EPSILON) continue; // ray is almost parallel // TODO: CHECK THIS OKAY?
                float t = num/denom;

                if(t<=0 || t >= 1) continue;

                // std::cout<<"num "<<(num)<<std::endl;
                // std::cout<<"denom "<<(denom)<<std::endl;
                // std::cout<<"t "<<(t)<<std::endl;

                Point intersect_point = x*t+(1-t)*y;
                float cos_theta = (intersect_point-p).dot(d);
                // std::cout<<"intersect_point "<<(intersect_point)<<std::endl;
                // std::cout<<"cos_theta "<<(cos_theta)<<std::endl;

                if(cos_theta<0) continue; // opposite to ray direction


                num_intersections++;
            }
            // std::cout<<"num_intersections "<<(num_intersections)<<std::endl;


            return ((num_intersections%2)==1);
        }

        std::optional<std::pair<float,const Model*>> getIntersectionLengthAndPart(const Ray& r) const {
            if(!_plane) return {};
            auto intersection_part = _plane->getIntersectionLengthAndPart(r);
            if(!intersection_part) return {};
            Point interp = r.src+intersection_part.value().first*r.dir;
            if(isOnSurface(interp)) return intersection_part;
            return {};
        }

        std::optional<Ray> getNormal(const Point& p) const {
            if(!isOnSurface(p)) return {};
            return _plane->getNormal(p);
        }

        std::ostream& print(std::ostream& os) const {
            os<<"Polygon{NumPoints="<<_points.size()<<",plane:";
            if(!_plane) {
                return os<<"NULL}";
            }
            return os<<(*_plane)<<"}";
        }

};