#include <vector>
#include <limits>
#include <fstream>
#include <iostream>
#include "Image.h"
#include "Camera.h"
#include "Models.h"

using namespace std;

class RenderEngine {
    private:
        typedef Triple<float> Vector3f;

        const Camera& _cam;
        const Color _ambient;
        Image& _img;
        std::vector<const Model*> _models;
        std::vector<const Light*> _lights;
        const int max_trace_depth = 3;
    public:
        RenderEngine(const Camera& cam,Image& img, const std::vector<Model*>& models, const std::vector<Light*>& lights, const Color ambient):
            _cam{cam}, _img{img}, _models{models.begin(),models.end()}, _lights{lights.begin(),lights.end()}, _ambient{ambient}
        {}

        void addModel(const Model* model) {
            _models.push_back(model);
        }

        Color trace(Ray r, float refractive_index, int depth) {
            
            // cout<<"trace begin: "<<r<<" refIdx: "<<refractive_index<<" depth: "<<depth<<endl;

            const Model* closest_model = NULL;
            float closest_distance = std::numeric_limits<float>::infinity();
            for(auto mod: _models) {
                auto intersection = mod->getIntersectionLength(r);
                if(!intersection) continue;
                auto len = intersection.value();
                if(len < closest_distance) {
                    closest_distance = len;
                    closest_model = mod;
                }
            }

            if(!closest_model){
                // cout<<"Background hit! "<<Color(0,0.2,0.2)<<endl;
                return Color(0,0,0);// TODO: Change this to background 
            }
            auto intersection_point_true = r.src+(closest_distance)*r.dir;
            auto intersection_point = intersection_point_true - EPSILON*r.dir; // taking intersection point slightly outside of intersection
            auto normal = closest_model->getNormal(intersection_point_true);
            std::optional<Color> reflected;
            std::optional<Color> refracted;
            bool has_refracted = false;

            std::vector<std::pair<Color,Vector3f>> light_rays;
            
            // cout<<"Intersection point: "<<intersection_point<<endl;

            // shadow rays
            for(auto light: _lights) {
                auto shadow_ray = light->getRayToLight(intersection_point);
                bool is_occluded = false;
                for(auto mod: _models) {
                    auto intersection = mod->getIntersectionLength(shadow_ray);
                    if(intersection && intersection.value() < shadow_ray.length) {
                        is_occluded = true;
                        break;
                    }
                }
                if(is_occluded) {
                    // std::cout<<"IS OCCLUDED!!!!!"<<std::endl;
                }
                if(!is_occluded) {
                    light_rays.push_back(std::make_pair(light->getIntensity(),shadow_ray.dir));
                }
            }

            if(depth < max_trace_depth && normal) { // if recursion depth is not reached and normal is present
                // reflected ray
                auto reflected_ray = Model::getReflected(r,normal.value());
                // cout<<"Tracing reflected!"<<endl;
                reflected = trace(reflected_ray,refractive_index,depth+1);

                // refracted ray
                auto trans_refractive_index = closest_model->getRefractiveIndex();
                if(trans_refractive_index) {
                    auto refracted_ray = Model::getRefracted(r,normal.value(),refractive_index,trans_refractive_index.value());
                    // cout<<"Tracing refracted!"<<endl;                
                    refracted = trace(refracted_ray,trans_refractive_index.value(),depth+1);
                }
            }

            // negating r.dir so that direction is away from point of intersection
            Color final_intensity = closest_model->getIntensity(normal.value().dir,-r.dir,light_rays,&_ambient,reflected?(&reflected.value()):NULL,refracted?(&refracted.value()):NULL);

            // cout<<"final intensity: "<<final_intensity<<endl;
            return final_intensity;
        }

        void render() {
            const int width = _img.width;
            const int height = _img.height;
            for(int i=0;i<width;i++) {
                for(int j=0;j<height;j++) {
                    float x = ((float)i+0.5)/width;
                    float y = ((float)j+0.5)/height;
                    Ray r = _cam.getRay(x,y).value();
                    // Ray r(Point(0,0,0),Vector3f(0,0,-1));
                    Color c = trace(r,1,0);
                    // std::cout<<"Initial ray: "<<r<<" | Final Color: "<<c<<std::endl;
                    _img.set(i,j,c);
                }
            }
        }

        void writeImage(const std::string& path) {
            std::ofstream ofs(path, std::ios::out | std::ios::binary);
            _img.write(ofs);
        }
};