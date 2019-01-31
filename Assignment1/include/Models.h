#pragma once

#include <optional>
#include <iostream>
#include <cmath>
#include <cassert>
#include "DS.h"

const double EPSILON = 1e-4;

struct Material {
    typedef Triple<float> Vector3f;

    Vector3f Ka,Kd,Ks,Krg,Ktg;
    float refractive_index, specular_coeff;

    Material(): Ka{0},Kd{0},Ks{0},Krg{0},Ktg{0}, refractive_index{-1}, specular_coeff{1} {}
};

class Model {
    public:
        typedef Triple<float> Vector3f;
        
        const Material mat;

        std::optional<float> getRefractiveIndex(Vector3f incident, Vector3f normal) const {
            if((this->mat).refractive_index<0) return {};
            // if hit from inside the return refractive index as 1 for the outside world
            return ((incident.dot(normal)<0)? ((this->mat).refractive_index):1.0);
        }

        Model(Material mat): mat{mat} {
        }

        virtual ~Model() = default;
        virtual std::optional<float> getIntersectionLength(const Ray&) const = 0;
        virtual std::optional<Ray> getNormal(const Point&) const = 0; // returns outward normal
        
        /**
         * @param{normal} Normal ray with source at the point of contact
         * @param{incident} Incident ray with direction pointing towards the point of contact
         * @return {Ray} Reflected ray pointing away from point of contact with source at point of contact
         */
        static Ray getReflected(const Ray& incident, const Ray& normal) {
            const float angle = (normal.dir).dot(incident.dir);
            const bool isInside = angle > 0;
            const float abs_angle = fabs(angle);
            Vector3f true_normal = isInside?(-normal.dir):(normal.dir);
            Vector3f reflected_dir = (2*abs_angle*true_normal) + incident.dir;

            return Ray(normal.src+(true_normal*EPSILON),reflected_dir); // Made the starting point slightly outside along reflection dir
        }

        /**
         * @param{normal} Normal ray with source at the point of contact
         * @param{incident} Incident ray with direction pointing towards the point of contact
         * @return {Ray} Reflected ray pointing away from point of contact with source at point of contact
         */
        static Ray getRefracted(const Ray& incident, const Ray& normal, const float incident_ref_idx, const float transfer_ref_idx) {
            const float cos_theta_i = (normal.dir).dot(incident.dir);
            const bool isInside = cos_theta_i > 0;
            const float abs_cos_theta_i = fabs(cos_theta_i);
            const float abs_sin_theta_i = sqrt(1-(abs_cos_theta_i*abs_cos_theta_i));
            const float ref_ratio = incident_ref_idx/transfer_ref_idx;
            const float abs_sin_theta_t = (ref_ratio*abs_sin_theta_i);
            const float abs_cos_theta_t = sqrt(1-(abs_sin_theta_t*abs_sin_theta_t));

            Vector3f true_normal = isInside?(-normal.dir):(normal.dir);

            Vector3f refracted_dir = ((ref_ratio*(incident.dir + true_normal*abs_cos_theta_i)) - (abs_cos_theta_t*true_normal)).normalize();
            
            // std::cout<<"Normal src: "<<normal.src<<"RefractedDir: "<<refracted_dir<<" delta: "<<(refracted_dir*EPSILON)<<std::endl;
            return Ray(normal.src-(true_normal*EPSILON),refracted_dir); // Made the starting point slightly outside along refraction dir
        }

        /**
         * @param{normal} Normal direction at the point of color
         * @param{view} View direction pointing away from the point of color
         * @param{lights} pair of {intensity,dir} for light sourcer where dir is pointing away from point of color
         * @param{ambient} ambient intensity in the room
         * @param{reflected} reflected intensity at the point of color
         * @param{refracted} refracted intensity at the point of color
         */
        virtual Color getIntensity(const Vector3f& normal, const Vector3f& view, const std::vector<std::pair<Color,Vector3f>>& lights, const Color* ambient, const Color* reflected, const Color* refracted) const {
            auto view_corr = view;
            auto normal_corr = normal;
            if(view_corr.dot(normal_corr) < 0) {
                // std::cout<<"NORMAL AND VIEW OPPOSITE!!"<<std::endl;

                // we are vieweing wrong normal (maybe the ray in inside the object?)
                normal_corr = -normal_corr;
            }

            Color final_color(0,0,0);

            // assert(lights.size()==1);

            for(const auto pr: lights) {
                auto intensity = pr.first;
                auto incident = pr.second;
                float cos_theta = incident.dot(normal_corr);
                if(cos_theta < 0) {
                    // light strikes from opposite side
                    continue;
                }
                auto reflected = 2*(cos_theta)*normal_corr - incident; // some duplicate code with getReflected function

                final_color += ((this->mat).Kd)*intensity*cos_theta; //TODO: convert according to eigen
                float cos_alpha = reflected.dot(view_corr);
                if(cos_alpha > 0) {
                    final_color += ((this->mat).Ks)*intensity*pow(cos_alpha,(this->mat).specular_coeff); //TODO: convert according to eigen
                }
            }

            if (ambient) {
                final_color += ((this->mat).Ka)*(*ambient);
            }

            if (reflected) {
                final_color += ((this->mat).Krg)*(*reflected);
            }

            if (refracted) {
                final_color += ((this->mat).Ktg)*(*refracted);
            }

            return final_color;
        }
};

class Light {
    private:
        Point _center;
        Color _intensity;
    public:
        Light(Point center, Color intensity): _center{center}, _intensity{intensity} {}
        Ray getRayToLight(Point p) const {
            return Ray(p,_center-p);
        }
        Color getIntensity() const {
            return _intensity;
        }
        friend std::ostream& operator<<(std::ostream& os, const Light& lg) {
            return os<<"Light{center="<<lg._center<<",intensity="<<lg._intensity<<"}";
        }
};

class Sphere: public Model {
    private:
        const Point _center;
        const float _radius;
        const float _radius_sq;
    public:
        Sphere(Point center, float radius, Material mat): Model{mat}, _center{center}, _radius{radius}, _radius_sq{_radius*_radius}
        {}
        Sphere(const Sphere& s): Model{s.mat}, _center{s._center},_radius{s._radius},_radius_sq{s._radius_sq} {}
        Sphere& operator=(const Sphere& s) = delete;
        

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