#pragma once

#include <iostream>
#include <optional>
#include <vector>
#include "defs.h"

class Image {
   private:
    int _width, _height;
    std::vector<Color> _image;

   public:
    const int& width;
    const int& height;
    Image(int w, int h)
        : _width{w}, _height{h}, width{_width}, height{_height} {
        _image.resize(w * h);
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

    std::optional<Color> get(int image_i, int image_j) const;
    bool set(int image_i, int image_j, Color c);
    friend std::ostream& operator<<(std::ostream& os, const Image& img);
    std::ostream& write(std::ostream& os) const;
};