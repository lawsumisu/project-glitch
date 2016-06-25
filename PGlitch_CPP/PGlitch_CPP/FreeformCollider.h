#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <vector>
#include "Enumerators.h"
#include "RectUtility.h"
#include "Quadtree.h"

/// <summary>
/// Represents a collider whose shape is a free form polygon.
/// Immutable.
/// </summary>
class FreeformCollider{

private:
    //Fields

    //List of points that compose the outline of this polygonal collider.
    //RI: points.size() >= 3.
    std::vector<sf::Vector2f> points;

    //std::vector<size_t> sortedXCoordinates = {}, sortedYCoordinates = {};

    //Rectangle that represents a hull for this collider. Every point in points lies within this rectangle.
    sf::FloatRect maxBounds;
    std::vector<sf::FloatRect> boundsList = {};
    std::vector<size_t> boundIndices = {};
    CustomUtilities::RectSorter sorter;
    Quadtree quadtree;
    Polygon colliderShape;

public:
    //Constructor

    /// <summary>
    /// Builds a <see cref="PolygonalCollider"/> out the input points. 
    /// Requires that points.size() >= 3.
    /// </summary>
    FreeformCollider(std::vector<sf::Vector2f> points);
    FreeformCollider(sf::FloatRect rect);

    // ======= //
    // Methods //
    // ======= //

    /// <summary>
    /// Determines whether or not the input rect is intersecting with this collider.
    /// output.second is a point that represents where the rect's origin should be located 
    /// to not be colliding with this collider, given that is pushed outward in a direction denoted by type.
    /// </summary>
    std::pair<bool, float> intersects(const sf::Transform& T, const sf::FloatRect& rect, SurfaceType type, 
        bool findExterior = true) const;

    /// <summary>
    /// Determines whether or not the input line is intersecting with this collider.
    /// output.second is a value in [0,1] that represents where along the the line the earliest collision was found.
    /// If there is no found intersection, output.first = false.
    /// </summary>
    /// <param name="T"></param>
    /// <param name="line"></param>
    /// <returns></returns>
    std::pair<bool, float> intersects(const sf::Transform& T, const CustomUtilities::Segment& line)const;

    std::vector<CustomUtilities::Segment> intersects(const Polygon& shape, const sf::Transform& T) const;

    /// <summary>
    /// Checks if a point is located within this collider.
    /// </summary>
    /// <param name="v"></param>
    /// <returns></returns>
    bool contains(sf::Vector2f& v, const sf::Transform& localToWorld) const;

    std::vector<sf::Vector2f> findSurfacePoints(const sf::Transform& T, const sf::FloatRect& rect) const;

    std::vector<sf::Vector2f> findInteriorPoints(const sf::Transform& T, const sf::FloatRect& rect) const;
    /// <summary>
    /// Draws an outline of this collider. If debug is 'true', then also draws inner bboxes and local space outer bbox.
    /// </summary>
    void draw(const sf::Transform& info, sf::RenderTarget& target, sf::RenderStates states, const sf::Color& color, bool debug) const;
};
