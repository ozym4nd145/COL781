#include <vector>
#include <limits.h>
#include <fstream>
#include <iostream>
#include "Image.h"
#include "Camera.h"
#include "Models.h"


class RenderEngine {
    private:
        const Camera& _cam;
        Image& _img;
        std::vector<const Model*> _models;
    public:
        RenderEngine(const Camera& cam,Image& img, const std::vector<Model*>& models):
            _cam{cam}, _img{img}, _models{models.begin(),models.end()}
        {}

        void addModel(const Model* model) {
            _models.push_back(model);
        }

        void render() {
            const int width = _img.width;
            const int height = _img.height;
            for(int i=0;i<width;i++) {
                for(int j=0;j<height;j++) {
                    float x = ((float)i+0.5)/width;
                    float y = ((float)j+0.5)/height;
                    Ray r = _cam.getRay(x,y).value();
                    const Model* closest_model = NULL;
                    int closest_distance = INT_MAX;
                    for(auto mod: _models) {
                        auto intersection = mod->getIntersectionLength(r);
                        if(!intersection) continue;
                        auto len = intersection.value();
                        if(len < closest_distance) {
                            closest_distance = len;
                            closest_model = mod;
                        }
                    }
                    if(closest_model) {
                        _img.set(i,j,closest_model->getColor());
                    } else {
                        _img.set(i,j,Color(0));
                    }
                }
            }
        }

        void writeImage(const std::string& path) {
            std::ofstream ofs(path, std::ios::out | std::ios::binary);
            _img.write(ofs);
        }
};