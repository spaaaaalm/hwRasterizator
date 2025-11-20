#pragma once
#include "shader.h"
#include "mathTypes/matrix.h"

struct PhongShader : public IShader {
    
    Mat4 Model;
    Mat4 View;
    Mat4 Projection;
    Mat4 MVP; // Projection * View * Model

    // pos света и камеры
    Vec3f lightPos = {0,0,2};
    Vec3f cameraPos = {0,0,2};

    // varying-переменные
    Vec3f varying_world_pos[3];
    Vec3f varying_normal[3];
    Vec4f varying_clip[3];

  
    Vec3f *positions = nullptr;
    Vec3f *normals   = nullptr;

    PhongShader() {}

    // ivertex=0..2
    Vec4f vertex(int iface, int ivertex) override {
        Vec3f pos = positions[iface*3 + ivertex];
        Vec3f nor = normals[iface*3 + ivertex].normalize();

        varying_world_pos[ivertex] = pos;
        varying_normal[ivertex]    = nor;

        Vec4f p = {pos.x, pos.y, pos.z, 1.0f};
        Vec4f clip = MVP * p;

        varying_clip[ivertex] = clip;
        return clip;
    }

    bool fragment(const Vec3f &bar, Color &color) override {
        // Интерполяция 
        Vec3f world_pos =
              varying_world_pos[0] * bar.x +
              varying_world_pos[1] * bar.y +
              varying_world_pos[2] * bar.z;

        Vec3f N =
              varying_normal[0] * bar.x +
              varying_normal[1] * bar.y +
              varying_normal[2] * bar.z;
        N = N.normalize();

        // Векторы для освещения
        Vec3f L = (lightPos - world_pos).normalize();
        Vec3f V = (cameraPos - world_pos).normalize();
        Vec3f R = (N * (2.0f * (N * L)) - L).normalize(); // отражённый луч

        float ambient  = 0.15f;
        float diffuse  = std::max(0.0f, N * L);
        float specular = pow(std::max(0.0f, R * V), 32.0f);

        float intensity = ambient + 0.9f * diffuse + 0.5f * specular;

        intensity = std::min(1.0f, intensity);

        uint8_t c = uint8_t(255 * intensity);
        color = Color(c, c, c);

        return false; 
    }
};
