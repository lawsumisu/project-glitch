#pragma once

#include <vector>
#include <SFML/System/Vector2.hpp>

namespace CustomUtilities {
    /// <summary>
    /// Gets the sign of the input.
    /// Returns 1 if input is positive, -1 if negative, and 0 otherwise.
    /// </summary>
    /// <param name="f"></param>
    /// <returns></returns>
    int sign(float f);

    /// <summary>
    /// Estimates the least-squares error mx + b = y from a series of x-y coordinates.
    /// Returns a <see cref="Vector2f"/> v such that v.x = b and v.y = m.
    /// </summary>
    /// <param name="coordinates"></param>
    /// <returns></returns>
    sf::Vector2f linearRegression(std::vector<sf::Vector2f> coordinates);

    const float pi = 3.1415926f;

    /// Converts degrees to radians.
    /// </summary>
    float degToRad(float degree);

    /// <summary>
    /// Clamps a value between a min and a max. 
    /// Does not require the min and max values to be ordered.
    /// </summary>
    float clamp(float value, float f1, float f2);
}