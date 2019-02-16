#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <vector>

#include "Camera.h"
#include "DS.h"
#include "Image.h"
#include "Models.h"
#include "defs.h"
#include "utils.h"

using namespace std;

class RenderEngine {
   private:
    const Camera& _cam;
    const Background& _background;
    Image& _img;
    std::vector<const Model*> _models;
    std::vector<const Light*> _lights;
    const Color _ambient;
    const int max_trace_depth = 4;
    // const int max_trace_depth = 1;
    const int num_sample = 5;

   public:
    RenderEngine(const Camera& cam, Image& img, const Background& background,
                 const std::vector<Model*>& models,
                 const std::vector<Light*>& lights, const Color ambient)
        : _cam{cam},
          _background{background},
          _img{img},
          _models{models.begin(), models.end()},
          _lights{lights.begin(), lights.end()},
          _ambient{ambient} {}

    void addModel(const Model* model);
    pair<Color,std::vector<pair<Vector3f,Vector3f>>> trace(Ray r, float refractive_index, int depth);
    void render();
    void writeImage(const std::string& path);
};