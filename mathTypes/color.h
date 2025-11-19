#pragma once
#include <cstdint>

struct Color {
    uint8_t r,g,b,a;
    Color() : r(0),g(0),b(0),a(255) {}
    Color(uint8_t R,uint8_t G,uint8_t B,uint8_t A=255) : r(R),g(G),b(B),a(A) {}
};
