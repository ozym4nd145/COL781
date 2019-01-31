#pragma once

#include <iostream>
#include <optional>
#include <vector>
#include "DS.h"
#include "utils.h"

class Image {
   private:
    int _width, _height;
    std::vector<Color> _image;

   public:
    const int& width;
    const int& height;
    Image(int w, int h)
        : _width{w},
          _height{h},
          width{_width},
          height{_height} {
              _image.resize(w*h);
          }
    Image(const Image& i)
        : _width{i.width},
          _height{i.height},
          _image{i._image},
          width{_width},
          height{_height} {}
    Image& operator=(const Image& i) {
        _width = i.width;
        _height = i.height;
        _image = i._image;
        return *this;
    }

    std::optional<Color> get(int image_i, int image_j) const {
        if ((image_i >= width) || (image_j >= height)) return {};
        const int index =
            image_j * width + image_i;  // based on image coordinates
        return _image[index];
    }

    bool set(int image_i, int image_j, Color c) {
        if ((image_i >= width) || (image_j >= height)) return false;
        const int index =
            image_j * width + image_i;  // based on image coordinates
        _image[index] = c;
        return true;
    }

    friend std::ostream& operator<<(std::ostream& os, const Image& img) {
        return os << "Image{width=" << img.width << ",height=" << img.height
                  << "}";
    }

    std::ostream& write(std::ostream& os) const {
        os << "P6\n" << width << " " << height << "\n255\n";
        for (int i = 0; i < width * height; ++i) {
            os << (unsigned char)(std::min(float(1), _image[i][0]) * 255)
               << (unsigned char)(std::min(float(1), _image[i][1]) * 255)
               << (unsigned char)(std::min(float(1), _image[i][2]) * 255);
        }
        return os;
    }
};