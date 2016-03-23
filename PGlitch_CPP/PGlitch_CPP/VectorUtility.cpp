#include "stdafx.h"
#include "VectorUtility.h"

using namespace CustomUtilities;

float VectorUtility::magnitude(Vector2f& v) {
    return sqrtf(v.x*v.x + v.y*v.y);
}

float VectorUtility::angle(Vector2f& v1, Vector2f& v2) {
    return acosf(dot(v1, v2) / (magnitude(v1)*magnitude(v2)));
}

Vector2f VectorUtility::normalize(Vector2f& v) {
    return v / magnitude(v);
}
