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
    const Color _ambient;
    Image& _img;
    std::vector<const Model*> _models;
    std::vector<const Light*> _lights;
    const int max_trace_depth = 4;
    const int num_sample = 5;

   public:
    RenderEngine(const Camera& cam, Image& img,
                 const std::vector<Model*>& models,
                 const std::vector<Light*>& lights, const Color ambient)
        : _cam{cam},
          _img{img},
          _models{models.begin(), models.end()},
          _lights{lights.begin(), lights.end()},
          _ambient{ambient} {}

    void addModel(const Model* model);
    Color trace(Ray r, float refractive_index, int depth);
    void render();
    void writeImage(const std::string& path);
};