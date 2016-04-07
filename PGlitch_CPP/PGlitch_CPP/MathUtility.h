#pragma once

#include <math.h>
#include <vector>
#include <SFML/System/Vector2.hpp>

namespace CustomUtilities {
    struct MathUtility {
        /// <summary>
        /// Gets the sign of the input.
        /// Returns 1 if input is positive, -1 if negative, and 0 otherwise.
        /// </summary>
        /// <param name="f"></param>
        /// <returns></returns>
        static int sign(float f);

        /// <summary>
        /// Estimates the least-squares error mx + b = y from a series of x-y coordinates.
        /// Returns a <see cref="Vector2f"/> v such that v.x = b and v.y = m.
        /// </summary>
        /// <param name="coordinates"></param>
        /// <returns></returns>
        static sf::Vector2f linearRegression(std::vector<sf::Vector2f> coordinates);
    };
}