#pragma once
#include "shader.h"
#include "mathTypes/matrix.h"

struct PhongShader : public IShader {

    Mat4 Model      = Mat4::identity();
    Mat4 View       = Mat4::identity();
    Mat4 Projection = Mat4::identity();
    Mat4 MVP        = Mat4::identity(); //все вместе 

    Vec3f lightPos  = {0,0,2};
    Vec3f cameraPos = {0,0,2};

    Vec3f varying_world_pos[3];
    Vec3f varying_normal[3];

    Vec3f *positions = nullptr;
    Vec3f *normals   = nullptr;

    int currentVertexIndex = 0;

    Vec4f vertex(int iface, int vertexID) override {
        Vec3f pos = positions[currentVertexIndex];
        Vec3f nor = normals[currentVertexIndex].normalize();

        //для интерполяции
        varying_world_pos[vertexID] = pos;
        varying_normal[vertexID]    = nor;

        // мировая -> клип
        Vec4f p = { pos.x, pos.y, pos.z, 1.0f };
        return MVP * p;
    }

    bool fragment(const Vec3f &bar, Color &color) override {
        // интерполяция мировых координат
        Vec3f wp =
              varying_world_pos[0] * bar.x +
              varying_world_pos[1] * bar.y +
              varying_world_pos[2] * bar.z;

        // интерполяция нормали
        Vec3f N =
              varying_normal[0] * bar.x +
              varying_normal[1] * bar.y +
              varying_normal[2] * bar.z;
        N = N.normalize();

        // освещение
        Vec3f L = (lightPos - wp).normalize();
        Vec3f V = (cameraPos - wp).normalize();
        Vec3f R = (N * (2.0f * (N * L)) - L).normalize();

        float ambient  = 0.2f;
        float diffuse  = std::max(0.0f, N * L);
        float specular = pow(std::max(0.0f, R * V), 32.0f);

        float I = ambient + diffuse * 0.8f + specular * 0.5f;
        I = std::min(I, 1.0f);

        uint8_t c = uint8_t(I * 255);
        color = Color(c, c, c);

        return false;
    }
};
