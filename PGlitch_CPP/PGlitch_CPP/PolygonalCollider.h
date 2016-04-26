#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <vector>
#include "Enumerators.h"
#include "RectUtility.h"

/// <summary>
/// Represents a collider whose shape is a free form polygon.
/// Immutable.
/// </summary>
class PolygonalCollider{

private:
    //Fields

    //List of points that compose the outline of this polygonal collider.
    //RI: points.size() >= 3.
    std::vector<sf::Vector2f> points;

    std::vector<size_t> sortedXCoordinates = {}, sortedYCoordinates = {};

    //Rectangle that represents a hull for this collider. Every point in points lies within this rectangle.
    sf::FloatRect maxBounds;
    std::vector<sf::FloatRect> boundsList = {};
    std::vector<size_t> boundIndices = {};
    CustomUtilities::RectSorter sorter;

    /*sf::FloatRect applyInfoToRect(const Transform& info, const sf::FloatRect& rect) const;
    sf::FloatRect applyInverseInfoToRect(const Transform& info, const sf::FloatRect& rect) const;
    sf::Vector2f applyInfoToPoint(const sf::Vector2f& p, const Transform& info) const;
    sf::Vector2f applyInverseInfoToPoint(const sf::Vector2f& p, const Transform& info) const;*/


public:
    //Constructor

    /// <summary>
    /// Builds a <see cref="PolygonalCollider"/> out the input points. 
    /// Requires that points.size() >= 3.
    /// </summary>
    PolygonalCollider(std::vector<sf::Vector2f> points);
    PolygonalCollider(sf::FloatRect rect);

    // ======= //
    // Methods //
    // ======= //

    /// <summary>
    /// Determines whether or not the input rect is intersecting with this collider.
    /// output.second is a point that represents where the rect's origin should be located 
    /// to not be colliding with this collider, given that is push outward in a direction denoted by direction.
    /// </summary>
    std::pair<bool, float> intersects(const sf::Transform& T, const sf::FloatRect& rect, SurfaceType direction) const;

    std::vector<sf::Vector2f> findSurfacePoints(const sf::Transform& T, const sf::FloatRect& rect) const;
    /// <summary>
    /// Draws an outline of this collider. If debug is 'true', then also draws inner bboxes and local space outer bbox.
    /// </summary>
    void draw(const Transform& info, sf::RenderTarget& target, sf::RenderStates states, const sf::Color& color) const;
};
