#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Color.hpp>
#include <vector>
#include "Enumerators.h"

class PolygonalColliderInfo {
private:
    //Fields 

    sf::Vector2f _offset;
    sf::Vector2f _rotationalOffset;
    float _angle;


public:
    PolygonalColliderInfo(sf::Vector2f offset, float angle, sf::Vector2f rotationalOffset) :
        _offset(offset), _angle(angle), _rotationalOffset(rotationalOffset){}
    PolygonalColliderInfo() : PolygonalColliderInfo(sf::Vector2f(), 0, sf::Vector2f()){}
    sf::Vector2f offset() const { return _offset; }
    sf::Vector2f rotationalOffset() const { return _rotationalOffset; }
    float angle() const { return _angle; }
};
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

    //Rectangle that represents a hull for this collider. Every point in points lies within this rectangle.
    sf::FloatRect maxBounds;

    sf::FloatRect applyInfoToBounds(const PolygonalColliderInfo& info) const;
    sf::Vector2f applyInfoToPoint(const sf::Vector2f& p, const PolygonalColliderInfo& info) const;


public:
    //Constructor

    /// <summary>
    /// Builds a <see cref="PolygonalCollider"/> out the input points. 
    /// Requires that points.size() >= 3.
    /// </summary>
    PolygonalCollider(std::vector<sf::Vector2f> points);
    PolygonalCollider(sf::FloatRect rect);

    //Methods

    /// <summary>
    /// Determines whether or not the input rect is intersecting with this collider.
    /// output.second is a point that represents where the rect's origin should be located 
    /// to not be colliding with this collider, given that is push outward in a direction denoted by direction.
    /// </summary>
    std::pair<bool, sf::Vector2f> intersects(const PolygonalColliderInfo& info, const sf::FloatRect& rect, CollisionDirection direction) const;

    /// <summary>
    /// Draws an axis-aligned rectangle defined by the input <see cref="FloatRect"/>.
    /// </summary>
    void draw(const PolygonalColliderInfo& info, sf::RenderTarget& target, sf::RenderStates states, const sf::Color& color) const;
};
