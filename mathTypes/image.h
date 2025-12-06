#pragma once
#include <vector>
#include <fstream>
#include "color.h"

struct Image {
    int width, height;
    std::vector<Color> data;

    Image(int w, int h) : width(w), height(h), data(w*h) {}

    void setPixel(int x, int y, const Color &c) {
        if (x < 0 || x >= width || y < 0 || y >= height) return;
        data[y*width + x] = c;
    }

    
    void savePPM(const char *filename) {
        std::ofstream f(filename, std::ios::binary);
        f << "P6\n" << width << " " << height << "\n255\n";
        for (auto &c : data) {
            f.put(c.r);
            f.put(c.g);
            f.put(c.b);
        }
    }
};
