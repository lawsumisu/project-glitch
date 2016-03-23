#pragma once

#include <SFML/System/Vector2.hpp>
#include <math.h>

using namespace sf;
namespace CustomUtilities {

    struct VectorUtility {
        template<typename T> static T dot(Vector2<T>& v1, Vector2<T>& v2) {
            return v1.x*v2.x + v1.y*v2.y;
        }
        static float magnitude(Vector2f& v);

        static float angle(Vector2f& v1, Vector2f& v2);

        static Vector2f normalize(Vector2f& v);
    };
}