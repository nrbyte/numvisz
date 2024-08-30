#include "math.hpp"

void math::setOrtho(Matrix<4, 4>& m, float top, float right, float bottom,
                    float left, float near, float far)
{
    m.clear();
    m.put(0, 0, 2.0f / (right - left));
    m.put(1, 1, 2.0f / (top - bottom));
    m.put(2, 2, -2.0f / (far - near));

    m.put(0, 3, -(right + left) / (right - left));
    m.put(1, 3, -(top + bottom) / (top - bottom));
    m.put(2, 3, -(far + near) / (far - near));

    m.put(3, 3, 1.0f);
}

void math::setTranslate(Matrix<4, 4>& m, float x, float y, float z)
{
    math::setIdentity(m);

    m.put(0, 3, x);
    m.put(1, 3, y);
    m.put(2, 3, z);
}

void math::setScale(Matrix<4, 4>& m, float x, float y, float z)
{
    m.clear();

    m.put(0, 0, x);
    m.put(1, 1, y);
    m.put(2, 2, z);
    m.put(3, 3, 1);
}
