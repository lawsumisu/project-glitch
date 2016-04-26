#pragma once

#include <SFML/System/Vector2.hpp>
#include <sstream>
#include <tuple>

using namespace sf;
namespace CustomUtilities {

    template<typename T> static T dot(const Vector2<T>& v1, const Vector2<T>& v2) {
        return v1.x*v2.x + v1.y*v2.y;
    }
    float magnitude(const Vector2f& v);

    float angle(const Vector2f& v1, const Vector2f& v2);

    Vector2f normalize(const Vector2f& v);

    std::string toString(const Vector2f& v);

    /// <summary>
    /// Returns a new <see cref="Vector2f"/> who's x and y values are bounded between 2 values.
    /// Requires that vmin.x &lt; vmax.x and vmin.y &lt; max.y.
    /// </summary>
    Vector2f clamp(const Vector2f& v, const Vector2f& vmin, const Vector2f& vmax);

    /// <summary>
    /// Converts a polar coordinate to a cartesian coordinate.
    /// </summary>
    Vector2f toPoint(float radius, float angle);

    bool approximate(const Vector2f& v1, const Vector2f& v2);
}

namespace std {
    template <> struct std::hash<Vector2f> {
        size_t operator()(const Vector2f &v) const {
            return hash<float>()(v.x) ^ hash<float>()(v.y);
        }
    };
}