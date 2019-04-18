
#ifndef STB_IMAGE_CPP_HEADER
#define STB_IMAGE_CPP_HEADER

#include "stb_image.h"

#include <memory>
#include <string>
#include <tuple>

namespace stb {

class image {
public:
    explicit image(const char* filename, int num_components = 0)
        : data_{stbi_load(filename, &w, &h, &comp, num_components)}
    {}

    explicit image(const std::string& filename, int num_components = 0)
        : image(filename.c_str(), num_components)
    {}


    int width() const { return w; }

    int height() const { return h; }

    int num_components() const { return comp; }

    std::uint32_t get_rgb(int x, int y) const
    {
        int addr = (y * w + x) * comp;
        std::uint32_t r = data_[addr];
        std::uint32_t g = data_[addr + 1];
        std::uint32_t b = data_[addr + 2];
        return (r << 16)  + (g << 8) + b;
    }

    const std::uint8_t* data() const { return data_.get(); }

    explicit operator bool() { return bool(data_); }

private:
    struct deleter {
        void operator()(unsigned char* data) const {
            stbi_image_free(data);
        }
    };

    int w = 0;
    int h = 0;
    int comp = 0;

    std::unique_ptr<std::uint8_t[], deleter> data_ = nullptr;
};

}

#endif
