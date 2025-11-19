#pragma once
#include "mathTypes/math.h"
#include "mathTypes/color.h"

struct IShader {
    virtual ~IShader() = default;

    // какие треугольник и вершина
    // clip-space позицию
    virtual Vec4f vertex(int iface, int ivertex) = 0;

    // барицентрические координаты
    // цвет пикселя
    // отбросить пиксель (discard)
    virtual bool fragment(const Vec3f &bar, Color &color) = 0;
};
