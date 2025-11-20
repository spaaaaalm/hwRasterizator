#include <iostream>
#include <vector>
#include "mathTypes/math.h"
#include "mathTypes/matrix.h"
#include "mathTypes/image.h"
#include "rasterizer.h"
#include "phong_shader.h"
#include "objLoader.h"

int main() {
    const int W = 512;
    const int H = 512;

    Image frame(W, H);
    std::vector<float> zbuffer(W * H, -1e9f);

    Mesh mesh;
    if (!loadOBJ("tinker.obj", mesh)) {
        std::cout << "Failed to load OBJ\n";
        return 1;
    }

    std::cout << "Loaded " << mesh.positions.size()
        << " verts, " << mesh.indices.size() / 3 << " tris.\n";

    // Нормализуем МОДЕЛЬ к [-1..1]
    Vec3f minB(1e9,1e9,1e9), maxB(-1e9,-1e9,-1e9);
    for (auto &v : mesh.positions) {
        minB.x = std::min(minB.x, v.x);
        minB.y = std::min(minB.y, v.y);
        minB.z = std::min(minB.z, v.z);
        maxB.x = std::max(maxB.x, v.x);
        maxB.y = std::max(maxB.y, v.y);
        maxB.z = std::max(maxB.z, v.z);
    }

    Vec3f center = (minB + maxB) * 0.5f;
    Vec3f size = maxB - minB;
    float scale = 1.0f / std::max(size.x, std::max(size.y, size.z));

    for (auto &v : mesh.positions) {
        v = (v - center) * scale;
    }

    // Правильный шейдер
    PhongShader shader;
    shader.MVP = Mat4::identity();
    shader.positions = mesh.positions.data();
    shader.normals   = mesh.normals.data();
    shader.lightPos  = {0,0,2};
    shader.cameraPos = {0,0,2};

    // Правильный рендер через indices
    for (size_t f = 0; f < mesh.indices.size(); f += 3) {
        int i0 = mesh.indices[f+0];
        int i1 = mesh.indices[f+1];
        int i2 = mesh.indices[f+2];

        Vec4f clip[3];

        shader.currentVertexIndex = i0;
        clip[0] = shader.vertex(0, 0);

        shader.currentVertexIndex = i1;
        clip[1] = shader.vertex(0, 1);

        shader.currentVertexIndex = i2;
        clip[2] = shader.vertex(0, 2);

        triangle(clip, shader, frame, zbuffer);
    }

    frame.savePPM("out.ppm");
    return 0;
}
