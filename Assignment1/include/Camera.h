#pragma once

#include <optional>
#include <iostream>
#include "DS.h"

class Camera {
    typedef Triple<float> Vector3f;
    private:
        Ray _world_view;
        Ray _camera_view; // camera at origin, pointing to -z direction
        float _ar; //aspect ratio
        const float _fov_degree; // field of view in degrees
        float _x_correction; // correction factor for x
        float _y_correction; // correction factor for y
        const double _pi = 3.14159265358979323846;

        Ray transformCameraToWorld(const Ray& r) const {
            return r; // TODO fill this function
        }
    public:
        Camera(const Ray& view ,float ar ,float fov_degree):
            _world_view{view}, _camera_view(Point(0),Vector3f(0,0,-1)), _ar{ar}, _fov_degree{fov_degree} {
            // convert field of view to radians
            _x_correction = tan((_fov_degree*_pi)/360.0); // tan_inv(theta/2);
            _y_correction = _x_correction/_ar;
        }

        /**
         * i => [0,1] image x
         * j => [0,1] image y
         */
        std::optional<Ray> getRay(float i,float j) const {
            if(i>1.0 || j>1.0 || i<0 || j<0) return {}; // outside range
            float x = 2*i-1; // [-1,1]
            float y = 1-2*j; // [-1,1]
            float x_corr = x*_x_correction; // [-tan(theta/2),tan(theta/2)]
            float y_corr = y*_y_correction; // [-tan(theta/2)/ar,tan(theta/2)/ar]
            Ray r(_camera_view.src,Vector3f(x_corr,y_corr,-1));
            return transformCameraToWorld(r);
        }

        friend std::ostream& operator<<(std::ostream& os, const Camera& cam) {
            return os<<"Camera{aspect_ratio="<<cam._ar<<",fov="<<cam._fov_degree<<",view="<<cam._world_view<<"}";
        }
};