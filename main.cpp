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
    std::vector<float> zbuffer(W * H, std::numeric_limits<float>::max());

    Mesh mesh;
    if (!loadOBJ("tinker2.obj", mesh)) {
        std::cout << "Failed to load OBJ\n";
        return 1;
    }

    // --- нормализация модели ---
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

    // --- CAMERA ---
    Vec3f eye    = {0, 0, 2};
    Vec3f centerLook = {0, 0, 0};
    Vec3f up     = {0, 1, 0};

    PhongShader shader;
    shader.Model = Mat4::identity();
    shader.View = Mat4::lookAt(eye, centerLook, up);
    shader.Projection = Mat4::perspective(3.14159f / 3.0f, float(W)/H, 0.1f, 10.0f);
    shader.MVP = shader.Projection * shader.View * shader.Model;

    shader.positions = mesh.positions.data();
    shader.normals   = mesh.normals.data();
    shader.lightPos  = {0,0,2};
    shader.cameraPos = eye;

    // --- RENDER ---
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
    std::cout << "Saved out.ppm\n";
    return 0;
}
