#include <iostream>
#include "Camera.h"
#include "DS.h"
#include "Engine.h"
#include "Image.h"
#include "Models.h"
#include "defs.h"
#include "utils.h"

using namespace std;

int main(int argc, char** argv) {
    const int width = 512;
    const int height = 512;
    Image i1{width, height};

    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <Input JSON file>" << endl;
        exit(-1);
    }

    string in_filename = string(argv[1]);
    State state = get_state(in_filename);

    RenderEngine render_man(*(state.cam), i1, *(state.bg), state.models, state.lights,
                            Color(0.2, 0.2, 0.2));
    render_man.render();
    render_man.writeImage("./sphere.ppm");
    return 0;
}