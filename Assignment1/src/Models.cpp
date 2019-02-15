#include "Models.h"
#include <Eigen/Dense>
#include <cassert>
#include <cmath>
#include <iostream>
#include <optional>
#include "Camera.h"
#include "DS.h"
#include "defs.h"
#include "utils.h"

std::optional<float> Model::_getRefractiveIndex(Vector3f incident,
                                                Vector3f normal) const {
    if ((this->mat).refractive_index < 0) return {};
    // if hit from inside the return refractive index as 1 for the outside
    // world
    return ((incident.dot(normal) < 0) ? ((this->mat).refractive_index) : 1.0);
}

Ray Model::_getReflected(const Ray& incident, const Ray& normal) const {
    const float angle = (normal.dir).dot(incident.dir);
    const bool isInside = angle > 0;
    const float abs_angle = fabs(angle);
    Vector3f true_normal = isInside ? (-normal.dir) : (normal.dir);
    Vector3f reflected_dir = (2 * abs_angle * true_normal) + incident.dir;

    return Ray(normal.src + (true_normal * EPSILON),
               reflected_dir);  // Made the starting point slightly outside
                                // along reflection dir
}
Ray Model::_getRefracted(const Ray& incident, const Ray& normal,
                         const float incident_ref_idx,
                         const float transfer_ref_idx) const {
    const float cos_theta_i = (normal.dir).dot(incident.dir);
    const bool isInside = cos_theta_i > 0;
    const float abs_cos_theta_i = fabs(cos_theta_i);
    const float abs_sin_theta_i = sqrt(1 - (abs_cos_theta_i * abs_cos_theta_i));
    const float ref_ratio = incident_ref_idx / transfer_ref_idx;
    const float abs_sin_theta_t = (ref_ratio * abs_sin_theta_i);
    const float abs_cos_theta_t = sqrt(1 - (abs_sin_theta_t * abs_sin_theta_t));

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
Color Model::_getIntensity(const Vector3f& normal, const Vector3f& view,
                          const std::vector<std::pair<Color, Vector3f>>& lights,
                          const Color* ambient, const Color* reflected,
                          const Color* refracted, std::optional<Color> texture) const {
    Vector3f view_corr = view;
    Vector3f normal_corr = normal;
    if (view_corr.dot(normal_corr) < 0) {
        // std::cout<<"NORMAL AND VIEW OPPOSITE!!"<<std::endl;

        // we are vieweing wrong normal (maybe the ray in inside the
        // object?)
        normal_corr = -normal_corr;
    }

    Color final_color(0, 0, 0);

    Vector3f surface_property = (this->mat).Kd;
    if(texture) {
        surface_property = (surface_property)*0.5 + 0.5*texture.value();
    }

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
        // some duplicate code with _getReflected function
        final_color += (surface_property).cwiseProduct(intensity) * cos_theta;
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

std::optional<Color> Model::_getTexture(const Point& p) const {
    return {};
}

std::optional<float> Model::getRefractiveIndex(Vector3f incident,
                                               Vector3f normal) const {
    Vector3f transformed_incident =
        apply_transformation(incident, this->trans, true, true, false);
    Vector3f transformed_normal =
        apply_transformation(normal, this->trans, true, true, true);
    return this->_getRefractiveIndex(transformed_incident, transformed_normal);
}

std::optional<std::pair<float, const Model*>>
Model::getIntersectionLengthAndPart(const Ray& r) const {
    Ray transformed_ray = apply_transformation(r, this->trans, true, false);
    // std::cout<<r<<"  and transformed ray is "<<transformed_ray<<std::endl;
    auto el = this->_getIntersectionLengthAndPart(transformed_ray);
    if (!el) return {};

    Vector3f original_pt =
        transformed_ray.src + el.value().first * transformed_ray.dir;
    Vector3f world_point =
        apply_transformation(original_pt, this->trans, false, false, false);
    float dist = (r.src - world_point).norm();

    return std::make_pair(dist, el.value().second);
}

std::optional<Ray> Model::getNormal(const Point& p) const {
    Vector3f transformed_point =
        apply_transformation(p, this->trans, true, false, false);

    auto returned_ray = this->_getNormal(transformed_point);
    if (!returned_ray) return {};

    Ray r = returned_ray.value();
    return apply_transformation(r, this->trans, false, true);
};
bool Model::isOnSurface(const Point& p) const {
    Vector3f transformed_point =
        apply_transformation(p, this->trans, true, false, false);
    return this->_isOnSurface(transformed_point);
};

Ray Model::getReflected(const Ray& incident, const Ray& normal) const {
    Ray transformed_incident =
        apply_transformation(incident, this->trans, true, false);
    Ray transformed_normal =
        apply_transformation(normal, this->trans, true, true);

    auto returned_ray =
        this->_getReflected(transformed_incident, transformed_normal);
    return apply_transformation(returned_ray, this->trans, false, false);
}
Ray Model::getRefracted(const Ray& incident, const Ray& normal,
                        const float incident_ref_idx,
                        const float transfer_ref_idx) const {
    Ray transformed_incident =
        apply_transformation(incident, this->trans, true, false);
    Ray transformed_normal =
        apply_transformation(normal, this->trans, true, true);

    auto returned_ray =
        this->_getRefracted(transformed_incident, transformed_normal,
                            incident_ref_idx, transfer_ref_idx);
    return apply_transformation(returned_ray, this->trans, false, false);
}
Color Model::getIntensity(const Vector3f& normal, const Vector3f& view,
                          const std::vector<std::pair<Color, Vector3f>>& lights,
                          const Color* ambient, const Color* reflected,
                          const Color* refracted, std::optional<Color> texture) const {
    Vector3f transformed_normal =
        apply_transformation(normal, this->trans, true, true, true);
    Vector3f transformed_view =
        apply_transformation(view, this->trans, true, true, false);

    std::vector<std::pair<Color, Vector3f>> transformed_lights;
    for (auto el : lights) {
        Vector3f transformed_light_dir =
            apply_transformation(el.second, this->trans, true, true, true);
        transformed_lights.push_back({el.first, transformed_light_dir});
    }

    return this->_getIntensity(transformed_normal, transformed_view,
                               transformed_lights, ambient, reflected,
                               refracted,texture);
}
std::optional<Color> Model::getTexture(const Point& p) const {
    return this->_getTexture(apply_transformation(p,this->trans,true, false,false));
}