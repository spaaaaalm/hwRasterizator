#pragma once
#include "math.h"

struct Mat4 {
    float m[4][4]{};

    static Mat4 identity() {
        Mat4 r;
        for (int i=0;i<4;i++) r.m[i][i] = 1.f;
        return r;
    }

    Mat4 operator*(const Mat4 &b) const {
        Mat4 r;
        for (int i=0;i<4;i++)
            for (int j=0;j<4;j++)
                for (int k=0;k<4;k++)
                    r.m[i][j] += m[i][k] * b.m[k][j];
        return r;
    }

    Vec4f operator*(const Vec4f &v) const {
        Vec4f r;
        r.x = v.x*m[0][0] + v.y*m[0][1] + v.z*m[0][2] + v.w*m[0][3];
        r.y = v.x*m[1][0] + v.y*m[1][1] + v.z*m[1][2] + v.w*m[1][3];
        r.z = v.x*m[2][0] + v.y*m[2][1] + v.z*m[2][2] + v.w*m[2][3];
        r.w = v.x*m[3][0] + v.y*m[3][1] + v.z*m[3][2] + v.w*m[3][3];
        return r;
    }
};
