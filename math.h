#pragma once
#include <cmath>
#include <array>
//asdasd
struct Vec2f {
    float x, y;
    Vec2f() : x(0), y(0) {}
    Vec2f(float _x, float _y) : x(_x), y(_y) {}
    float& operator[](int i) { return i == 0 ? x : y; }
};

struct Vec3f {
    float x, y, z;
    Vec3f() : x(0), y(0), z(0) {}
    Vec3f(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    float& operator[](int i) {
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }

    Vec3f operator+(const Vec3f &b) const {
        return {x + b.x, y + b.y, z + b.z};
    }
    Vec3f operator-(const Vec3f &b) const {
        return {x - b.x, y - b.y, z - b.z};
    }
    Vec3f operator*(float f) const {
        return {x*f, y*f, z*f};
    }
    float operator*(const Vec3f &b) const { // dot
        return x*b.x + y*b.y + z*b.z;
    }
    Vec3f cross(const Vec3f &b) const {
        return {
            y*b.z - z*b.y,
            z*b.x - x*b.z,
            x*b.y - y*b.x
        };
    }
    Vec3f normalize() const {
        float l = std::sqrt(x*x + y*y + z*z);
        return {x/l, y/l, z/l};
    }
};

struct Vec4f {
    float x,y,z,w;
    Vec4f() : x(0),y(0),z(0),w(1) {}
    Vec4f(float X,float Y,float Z,float W=1) : x(X),y(Y),z(Z),w(W) {}
    float& operator[](int i) {
        if (i==0) return x;
        if (i==1) return y;
        if (i==2) return z;
        return w;
    }
};
