#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <iostream>

using namespace sf;
namespace CustomUtilities {

    struct RectUtility {
        template <typename T> static std::string toString(Rect<T> R) {
            std::stringstream ss;
            ss << "Rect:origin = (" << R.left << "," << R.top << "); width = " 
                << R.width << "; height = " << R.height;
            return ss.str();
        }
        static FloatRect construct(Vector2f center, Vector2f dimensions);
        template <typename T> static Rect<T> copy(Rect<T> R) {
            return Rect(R.left, R.top, R.width, R.height);
        }
    };
}

