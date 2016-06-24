#pragma once
#include "LineUtility.h"

/// <summary>
/// Represents the Ax + By = C form of a line. Immutable
/// </summary>
class Line {
    float a, b, c;

public:
    //Constructor
    Line(float a, float b, float c);
    Line(sf::Vector2f& p1, sf::Vector2f& p2);

    // ======= //
    // Methods //
    // ======= //

    /// <summary>
    /// Gets the line the input is a segment of.
    /// </summary>
    static Line toLine(CustomUtilities::Segment& segment);

    /// <summary>
    /// Gets a value that corresponds to which side of the line a specific point is.
    /// Returns -1 if Ax + By &lt; C, 0 if Ax + By = C, and 1 if Ax + By &gt; C.
    /// </summary>
    /// <param name="v"></param>
    /// <returns></returns>
    int sign(sf::Vector2f& v) const;

    /// <summary>
    /// Gets the line that is parallel to this one and passes through the input point.
    /// </summary>
    Line getParallelLineTo(sf::Vector2f& v) const;

    float A();
    float B();
    float C();
};
