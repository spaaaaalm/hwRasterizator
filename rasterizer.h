#pragma once
#include <vector>
#include <algorithm>
#include <limits>
#include "mathTypes/math.h"
#include "shader.h"
#include "mathTypes/image.h"



// Более надёжная barycentric (через векторные произведения)
inline Vec3f barycentric_fast(const Vec3f *pts, const Vec3f &P) {
    // Using cross to compute areas
    Vec3f A = { pts[1].x - pts[0].x, pts[1].y - pts[0].y, 0.0f };
    Vec3f B = { pts[2].x - pts[0].x, pts[2].y - pts[0].y, 0.0f };
    Vec3f C = { P.x - pts[0].x,     P.y - pts[0].y,     0.0f };

    float denom = A.x * B.y - A.y * B.x;
    if (std::fabs(denom) < 1e-9f) return {-1,1,1};

    float u = (C.x * B.y - C.y * B.x) / denom;
    float v = (A.x * C.y - A.y * C.x) / denom;
    float w = 1.0f - u - v;
    return {w, u, v}; // corresponds to weights for pts[0], pts[1], pts[2]
}

// triangle: clip space -> rasterize
inline void triangle(const Vec4f clip[3], IShader &shader, Image &frame, std::vector<float> &zbuffer) {
    int width = frame.width;
    int height = frame.height;

    // Convert clip -> NDC (divide by w)
    Vec3f ndc[3];
    float inv_w[3];
    for (int i = 0; i < 3; ++i) {
        if (std::fabs(clip[i].w) < 1e-9f) {
            ndc[i] = {0,0,0};
            inv_w[i] = 1e9f;
        } else {
            inv_w[i] = 1.0f / clip[i].w;
            ndc[i] = { clip[i].x * inv_w[i], clip[i].y * inv_w[i], clip[i].z * inv_w[i] };
        }
    }

    // Screen coordinates
    Vec3f screen[3];
    for (int i = 0; i < 3; ++i) {
        float x = (ndc[i].x + 1.0f) * 0.5f * (float)width;
        float y = (ndc[i].y + 1.0f) * 0.5f * (float)height;
        // note: image origin at top-left; we keep same orientation (y increases downward)
        screen[i] = { x, y, ndc[i].z };
    }

    // Bounding box in integer pixel coords
    int minx = std::max(0, (int)std::floor(std::min({screen[0].x, screen[1].x, screen[2].x})));
    int maxx = std::min(width-1, (int)std::ceil (std::max({screen[0].x, screen[1].x, screen[2].x})));
    int miny = std::max(0, (int)std::floor(std::min({screen[0].y, screen[1].y, screen[2].y})));
    int maxy = std::min(height-1, (int)std::ceil (std::max({screen[0].y, screen[1].y, screen[2].y})));

    // Iterate pixels in bbox
    Vec3f P;
    for (int y = miny; y <= maxy; ++y) {
        for (int x = minx; x <= maxx; ++x) {
            P.x = (float)x + 0.5f; // center of pixel
            P.y = (float)y + 0.5f;
            // compute barycentric in screen space
            Vec3f bc = barycentric_fast(screen, P);
            if (bc.x < -1e-4f || bc.y < -1e-4f || bc.z < -1e-4f) continue; // outside

            // Perspective-correct weights:
            float w0 = bc.x * inv_w[0];
            float w1 = bc.y * inv_w[1];
            float w2 = bc.z * inv_w[2];
            float wsum = w0 + w1 + w2;
            if (wsum == 0.0f) continue;
            Vec3f bar = { w0 / wsum, w1 / wsum, w2 / wsum }; // corrected barycentrics for fragment shader

            // Interpolated depth (we use ndc.z with perspective correction)
            float depth = (ndc[0].z * (bc.x * inv_w[0]) + ndc[1].z * (bc.y * inv_w[1]) + ndc[2].z * (bc.z * inv_w[2])) / wsum;

            int idx = y * width + x;
            // zbuffer initial values should be very small (we expect depth usually in [-1..1], nearer = bigger)
            if (depth <= zbuffer[idx]) continue;
            // call fragment shader
            Color color;
            bool discard = shader.fragment(bar, color);
            if (!discard) {
                zbuffer[idx] = depth;
                frame.setPixel(x, y, color);
            }
        }
    }
}
