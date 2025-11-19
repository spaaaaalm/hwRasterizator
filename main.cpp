#include <iostream>
#include "mathTypes/image.h"
#include "phong_shader.h"

int main() {
    std::cout << "Starting test...\n";

    Image img(256, 256);

    PhongShader shader;


    Vec3f fakePositions[3] = {
        {0,0,0},
        {0,0,0},
        {0,0,0}
    };
    Vec3f fakeNormals[3] = {
        {0,0,1},
        {0,0,1},
        {0,0,1}
    };

    shader.positions = fakePositions;
    shader.normals   = fakeNormals;

    Vec3f bar = {0.33f, 0.33f, 0.34f};
    Color col;

    bool discard = shader.fragment(bar, col);
    if (discard) {
        std::cout << "Fragment discarded!\n";
    }

    std::cout << "fragment color = "
              << (int)col.r << " "
              << (int)col.g << " "
              << (int)col.b << "\n";

    for (int y = 64; y < 192; y++) {
        for (int x = 64; x < 192; x++) {
            img.setPixel(x, y, col);
        }
    }


    img.savePPM("test.ppm");

    std::cout << "Saved test.ppm\n";
    std::cout << "Open it with any viewer that supports PPM.\n";
    return 0;
}
