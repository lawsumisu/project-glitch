#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Color.hpp>
#include <utility>
#include <string>
#include <vector>
#include <unordered_set>
/// <summary>
/// Represents a directed line segment between two 2D points.
/// </summary>
class Segment {
private:
    //Fields

    sf::Vector2f p, q;

    //Methods

    static float cross(sf::Vector2f& v1, sf::Vector2f& v2);

public:
    //Constructors

    Segment(const sf::Vector2f& p1, const sf::Vector2f& p2);
    Segment(const sf::Vector2f& origin, float angle, float distance);

    //Methods
    /// <summary>
    /// Checks for intersection with another line. If there is an intersection, 
    /// output.first == true and output.second will be a value t (0 &lt;= t &lt;= 1) 
    /// such that atPoint(t) corresponds to the earliest found intersection. 
    /// 
    /// If isInfiniteLine == true, then this line is treated as one of infinite length passing through 
    /// p1 and p2, resulting in potential intersections with t &lt; 0 or t &gt; 1.
    /// 
    /// If there is no intersection, then intersects.first == false. 
    /// However, if isInfiniteLine, then t 
    /// </summary>
    /// <param name="line"></param>
    /// <returns></returns>
    std::pair<bool, float> intersects(const Segment& line, bool isInfiniteLine = false) const;

    std::string toString() const;

    /// <summary>
    /// Returns a point the lies on this line between p1 and p2.
    /// Requires that 0 &lt;= t &lt;= 1.
    /// </summary>
    /// <param name="t"></param>
    /// <returns></returns>
    sf::Vector2f atPoint(float t) const;


    /// <summary>
    /// Get a t value such that atPoint(t) = v.
    /// Requires that v lie along the line defined by this <see cref="Line"/>.
    /// </summary>
    /// <param name="v"></param>
    /// <returns></returns>
    float getT(sf::Vector2f& v) const;

    /// <summary>
    /// Returns a new <see cref="Line"/> that corresponds to this line's p1 being translated to v.
    /// </summary>
    /// <param name="v"></param>
    /// <returns></returns>
    Segment moveP1(const sf::Vector2f& v) const;

    sf::FloatRect bounds() const;

    std::pair<bool, float> intersects(const sf::FloatRect& rect) const;

    /// <summary>
    /// Returns a list of points where this line intersects with an input rect.
    /// </summary>
    std::unordered_set<sf::Vector2f> findAllIntersections(const sf::FloatRect& rect) const;

    /// <summary>
    /// Tells which endpoints of this line are within an input rect.
    /// The returned list can have 0, 1, or 2 points.
    /// </summary>d
    std::unordered_set<sf::Vector2f> findInteriorPoints(const sf::FloatRect& rect) const;

    /// <summary>
    /// Finds the portion of this line that lies within a given rectangle.
    /// If there is no inner line, then output.first == false.
    /// </summary>
    /// <param name="rect"></param>
    /// <returns></returns>
    std::pair<bool, Segment> findInnerLine(const sf::FloatRect& rect) const;

    Segment transform(const sf::Transform& T) const;

    sf::Vector2f start() const { return p; }
    sf::Vector2f end() const { return q; }

    bool operator==(const Segment& otherSegment) const;

    void draw(const sf::Color& color, sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const;

    friend Segment operator*(const Segment& line, float f);
    friend std::ostream& operator<<(std::ostream& os, const Segment& shape);

};