#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Color.hpp>
#include <utility>
#include <string>
#include <vector>
#include <unordered_set>

namespace CustomUtilities {

    class Line {
    private:
        //Fields

        sf::Vector2f p1, p2;

        //Methods

        static float cross(sf::Vector2f& v1, sf::Vector2f& v2);

    public:
        //Constructors

        Line(const sf::Vector2f& p1, const sf::Vector2f& p2);
        Line(const sf::Vector2f& origin, float angle, float distance);

        //Methods
        /// <summary>
        /// Checks for intersection with another line. If there is an intersection, 
        /// output.first == true and output.second will be a value t (0 &lt;= t &lt;= 1) 
        /// such that atPoint(t) corresponds to the earliest found intersection.
        /// 
        /// If there is no intersection, then intersects.first == false.
        /// </summary>
        /// <param name="line"></param>
        /// <returns></returns>
        std::pair<bool, float> intersects(const Line& line) const;

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
        Line moveP1(const sf::Vector2f& v) const;

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
        std::pair<bool, Line> findInnerLine(const sf::FloatRect& rect) const;

        sf::Vector2f start() const { return p1; }
        sf::Vector2f end() const { return p2; }
        void draw(const sf::Color& color, sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const;

        friend Line operator*(const Line& line, float f);

    };

}