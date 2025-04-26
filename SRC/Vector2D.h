// Vector2D.h
#include <corecrt_math.h>
#ifndef __VECTOR2D_H__
#define __VECTOR2D_H__

class Vector2D
{
public:
    float x;
    float y;

    Vector2D() : x(0), y(0) {}
    Vector2D(float _x, float _y) : x(_x), y(_y) {}

    Vector2D operator+(const Vector2D& v) const { return Vector2D(x + v.x, y + v.y); }
    Vector2D operator-(const Vector2D& v) const { return Vector2D(x - v.x, y - v.y); }
    Vector2D operator*(float scalar) const { return Vector2D(x * scalar, y * scalar); }

    float Dot(const Vector2D& v) const { return x * v.x + y * v.y; }
    float Length() const { return sqrt(x * x + y * y); }
    Vector2D Normalize() const {
        float len = Length();
        if (len == 0) return Vector2D(0, 0);
        return Vector2D(x / len, y / len);
    }
};

#endif
