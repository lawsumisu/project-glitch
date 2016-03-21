#pragma once

#include <math.h>
namespace Physics {

    struct Vector2 {
        const float x, y;
        

        Vector2(float x, float y);
        float magnitude() const;
        const Vector2 newX(float x) const;
        const Vector2 newY(float y) const;
        const Vector2 operator+(const Vector2& v) const;
        const Vector2 operator-(const Vector2& v) const;
        const Vector2 operator*(float f) const;

    private:
        mutable float _magnitude = -1;
    };
}

using namespace Physics;

Vector2::Vector2(float x, float y) : x(x), y(y){}

float Vector2::magnitude() const {
    if (_magnitude == -1) _magnitude = sqrtf(x*x + y*y);
    return _magnitude;
}

const Vector2 Vector2::newX(float newX) const {
    return Vector2(newX, y);
}

const Vector2 Vector2::newY(float newY) const {
    return Vector2(x, newY);
}

const Vector2 Vector2::operator+(const Vector2& v) const {
    return Vector2(x + v.x, y + v.y);
}

const Vector2 Vector2::operator*(float f) const {
    return Vector2(x*f, y*f);
}

const Vector2 Vector2::operator-(const Vector2& v) const {
    return Vector2(x - v.x, y - v.y);
}
