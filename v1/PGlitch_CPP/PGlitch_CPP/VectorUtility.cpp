#include "stdafx.h"
#include <algorithm>
#include <math.h>
#include "VectorUtility.h"

using namespace CustomUtilities;

float CustomUtilities::magnitude(const Vector2f& v) {
    return sqrtf(v.x*v.x + v.y*v.y);
}

float CustomUtilities::angle(const Vector2f& v1, const Vector2f& v2) {
    return acosf(dot(v1, v2) / (magnitude(v1)*magnitude(v2)));
}

Vector2f CustomUtilities::normalize(const Vector2f& v) {
    return v / magnitude(v);
}

std::string CustomUtilities::toString(const Vector2f& v) {  
    std::stringstream ss;
    ss << "Vector2f: (" << v.x << "," << v.y << ")";
    return ss.str();
}

Vector2f CustomUtilities::clamp(const Vector2f& v, const Vector2f& vmin, const Vector2f& vmax) {
    return Vector2f(std::min(std::max(v.x, vmin.x), vmax.x), std::min(std::max(v.y, vmin.y), vmax.y));
}

Vector2f CustomUtilities::toPoint(float radius, float angle) {
    return Vector2f(cosf(angle), sinf(angle)) * radius;
}

bool CustomUtilities::approximate(const Vector2f& v1, const Vector2f& v2) {
    return magnitude(v1 - v2) < 0.001;
}