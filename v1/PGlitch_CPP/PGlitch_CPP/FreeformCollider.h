#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <vector>
#include "Enumerators.h"
#include "RectUtility.h"
#include "Polygon.h"

/// <summary>
/// Represents a collider whose shape is a free form polygon.
/// Immutable.
/// </summary>
class FreeformCollider{

private:
    //Fields
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

    std::vector<Segment> intersects(const Polygon& shape, const sf::Transform& T) const;

    /// <summary>
    /// Checks if a point is located within this collider.
    /// </summary>
    /// <param name="v"></param>
    /// <returns></returns>
    bool contains(sf::Vector2f& v, const sf::Transform& localToWorld) const;

    /// <summary>
    /// Draws an outline of this collider. If debug is 'true', then also draws inner bboxes and local space outer bbox.
    /// </summary>
    void draw(const sf::Transform& info, sf::RenderTarget& target, sf::RenderStates states, const sf::Color& color, bool debug) const;
};
