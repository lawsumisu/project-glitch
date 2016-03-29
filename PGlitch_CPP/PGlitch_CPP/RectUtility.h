#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Color.hpp>
#include <iostream>

using namespace sf;
namespace CustomUtilities {

    struct RectUtility {
        template <typename T> static std::string toString(const Rect<T>& R) {
            std::stringstream ss;
            ss << "Rect:origin = (" << R.left << "," << R.top << "); width = " 
                << R.width << "; height = " << R.height;
            return ss.str();
        }
        static FloatRect construct(Vector2f& center, Vector2f& dimensions);
        template <typename T> static Rect<T> copy(Rect<T> R) {
            return Rect<T>(R.left, R.top, R.width, R.height);
        }

        /// <summary>
        /// Draws an axis-aligned rectangle defined by the input <see cref="FloatRect"/>.
        /// </summary>
        static void draw(FloatRect&R, Color& color, sf::RenderTarget& target, sf::RenderStates states);
    };
}

