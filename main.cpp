#include <iostream>
#include <vector>
#include "math.h"
#include "mathTypes/image.h"
#include "rasterizer.h"
#include "phong_shader.h"
#include "mathTypes/matrix.h"

int main() {
    const int W = 512, H = 512;
    Image frame(W, H);

    // инициализация zbuffer - очень маленькие значения (чтобы depth > zbuffer для видимых)
    std::vector<float> zbuffer(W * H, -1e9f);

    // Простой треугольник в "мировых" координатах.
    // Мы упростили: используем MVP = identity, поэтому позиции должны быть в NDC (x,y in [-1,1], z in [-1,1])
    // Но PhongShader использует world_pos как positions[], так что positions[] = world_pos.
    Vec3f positions[3] = {
        {-0.6f, -0.6f,  0.0f}, // левый-низ
        { 0.6f, -0.6f,  0.0f}, // правый-низ
        { 0.0f,  0.6f,  0.0f}  // верх
    };
    Vec3f normals[3] = {
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 1.0f, 1.0f}
    };

    PhongShader shader;
    // Используем identity для MVP — тогда clip = world_pos (w=1)
    Mat4 I = Mat4::identity();
    shader.MVP = I;
    shader.positions = positions;
    shader.normals = normals;
    // Свет и камера спереди
    shader.lightPos = {0.0f, 0.0f, 1.0f};
    shader.cameraPos = {0.0f, 0.0f, 1.0f};

    // build clip coords for the single "face" (iface = 0)
    Vec4f clip[3];
    // PhongShader.vertex expects positions arranged per-face as positions[iface*3 + ivertex]
    // поэтому создадим временную layout: одна face, positions расположены подряд — простой обход:
    // Но у нас shader.vertex uses positions[iface*3 + ivertex], so let's arrange positions array accordingly.
    // Мы заранее подготовили positions[] как contiguous (0..2), и будем передавать iface=0.
    for (int v = 0; v < 3; ++v) {
        clip[v] = shader.vertex(0, v);
    }

    // rasterize one triangle
    triangle(clip, shader, frame, zbuffer);

    // Сохраняем картинку
    frame.savePPM("out.ppm");
    std::cout << "Saved out.ppm (512x512). Откройте файл, должен быть серый треугольник.\n";
    return 0;
}
