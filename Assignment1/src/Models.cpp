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

std::optional<float> Model::getRefractiveIndex(Vector3f incident,
                                               Vector3f normal) const {
    if ((this->mat).refractive_index < 0) return {};
    // if hit from inside the return refractive index as 1 for the outside
    // world
    return ((incident.dot(normal) < 0) ? ((this->mat).refractive_index) : 1.0);
}

Ray Model::getReflected(const Ray& incident, const Ray& normal) {
    const float angle = (normal.dir).dot(incident.dir);
    const bool isInside = angle > 0;
    const float abs_angle = fabs(angle);
    Vector3f true_normal = isInside ? (-normal.dir) : (normal.dir);
    Vector3f reflected_dir = (2 * abs_angle * true_normal) + incident.dir;

    return Ray(normal.src + (true_normal * EPSILON),
               reflected_dir);  // Made the starting point slightly outside
                                // along reflection dir
}
Ray Model::getRefracted(const Ray& incident, const Ray& normal,
                        const float incident_ref_idx,
                        const float transfer_ref_idx) {
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
Color Model::getIntensity(const Vector3f& normal, const Vector3f& view,
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

Color Model::getTexture(const Color& intensity, const Point& p) const {
    return intensity;
}