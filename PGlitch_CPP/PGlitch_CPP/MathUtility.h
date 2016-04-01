#pragma once

#include <math.h>
#include <vector>
#include <SFML/System/Vector2.hpp>

namespace CustomUtilities {
    struct MathUtility {
        /// <summary>
        /// Tells if the input is negative or not.
        /// Returns 1 if input is nonnegative, -1 otherwise.
        /// </summary>
        /// <param name="f"></param>
        /// <returns></returns>
        static int sign(float f) { return f == abs(f) ? 1 : -1; }

        /// <summary>
        /// Estimates the least-squares error mx + b = y from a series of x-y coordinates.
        /// Returns a <see cref="Vector2f"/> v such that v.x = b and v.y = m.
        /// </summary>
        /// <param name="coordinates"></param>
        /// <returns></returns>
        static sf::Vector2f linearRegression(std::vector<sf::Vector2f> coordinates);
    };
}