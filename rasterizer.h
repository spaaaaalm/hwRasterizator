#pragma once
#include <vector>
#include <algorithm>
#include <limits>
#include "mathTypes/math.h"
#include "shader.h"
#include "mathTypes/image.h"


inline Vec3f barycentric_fast(const Vec3f *pts, const Vec3f &P) {

    Vec3f A = { pts[1].x - pts[0].x, pts[1].y - pts[0].y, 0.0f };
    Vec3f B = { pts[2].x - pts[0].x, pts[2].y - pts[0].y, 0.0f };
    Vec3f C = { P.x - pts[0].x,     P.y - pts[0].y,     0.0f };

    float denom = A.x * B.y - A.y * B.x;
    if (std::fabs(denom) < 1e-9f) return {-1,1,1};

    float u = (C.x * B.y - C.y * B.x) / denom;
    float v = (A.x * C.y - A.y * C.x) / denom;
    float w = 1.0f - u - v;
    return {w, u, v}; 
}

void triangle(Vec4f clip[3], IShader &shader, Image &image, std::vector<float> &zbuf) {
    const int W = image.width;
    const int H = image.height;

    Vec3f ndc[3];
    for (int i = 0; i < 3; i++) {
        float w = clip[i].w;
        ndc[i] = { clip[i].x / w, clip[i].y / w, clip[i].z / w };
    }

    Vec3f pts[3];
    for (int i = 0; i < 3; i++) {
        pts[i].x = int((ndc[i].x + 1.f) * 0.5f * W);
        pts[i].y = int((-ndc[i].y + 1.f) * 0.5f * H);
        pts[i].z = 0.f;
    }

    int minx = W-1, miny = H-1;
    int maxx = 0,    maxy = 0;
    for (int i = 0; i < 3; i++) {
        minx = std::max(0, std::min(minx, int(pts[i].x)));
        miny = std::max(0, std::min(miny, int(pts[i].y)));
        maxx = std::min(W-1, std::max(maxx, int(pts[i].x)));
        maxy = std::min(H-1, std::max(maxy, int(pts[i].y)));
    }

    for (int y = miny; y <= maxy; y++) {
        for (int x = minx; x <= maxx; x++) {

            Vec3f bc = barycentric_fast(pts, Vec3f{float(x), float(y), 0.f});
            if (bc.x < 0 || bc.y < 0 || bc.z < 0) continue;

            float z = 1.0f / (bc.x / clip[0].w + bc.y / clip[1].w + bc.z / clip[2].w);
            float w0 = bc.x / clip[0].w;
            float w1 = bc.y / clip[1].w;
            float w2 = bc.z / clip[2].w;
            float w_sum = w0 + w1 + w2;
            Vec3f bc_correct = { w0 / w_sum, w1 / w_sum, w2 / w_sum };

            float z_clip = clip[0].z * bc_correct.x + clip[1].z * bc_correct.y + clip[2].z * bc_correct.z;
            float z_ndc = z_clip / z;

            int index = x + y * W;
            if (z_clip > zbuf[index]) continue;
            zbuf[index] = z_clip;
            
            Color color;
            if (!shader.fragment(bc_correct, color)) {
                image.setPixel(x, y, color);
            }
        }
    }
}
