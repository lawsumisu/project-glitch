#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>
#include "LineUtility.h"
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transform.hpp>
#include "Enumerators.h"
#include "Polygon.h"

using namespace sf;
using namespace std;
using namespace CustomUtilities;

namespace Physics {
    
    /// <summary>
    /// Represents a collider whose shape is that of a series of same-width rectangles or "pillars".
    /// </summary>
    class PillarCollider {
    private:
        float _width, maxHeight, maxDepth;
        sf::Vector2f _origin;
        size_t size;
        std::vector<sf::FloatRect> pillars;
        sf::FloatRect maxBounds;
      
        // ======= //
        // Methods //
        // ======= //

        /// <summary>
        /// Gets the minimum and maximum indices that correspond to the pillars that the specified bounds could potentially intersect within
        /// the pillar vector.
        /// If the bounds have no potential intersection at all, returns {-1,-1}. 
        /// </summary>
        /// <param name="bounds"></param>
        /// <returns></returns>
        std::pair<int, int> getBoundIndices(sf::FloatRect& bounds) const;

    public:
        // ======= //
        // Methods //
        // ======= //

        /// <summary>
        /// Initalizes a <see cref="MaskCollider"/> object with origin origin, whose shape is a series of rectangles
        /// all of width width. Within the collider, a rectangle r[i] has a length == depthMap[i] - heightMap[i].
        /// Requires that heightMap.size() == depthMap.size() and both be >= 1.
        /// Requires that depthMap[i] >= heightMap[i].
        /// *Follows the convention that positive y-coordinates from y = 0 correspond to "down" and 
        /// negative y-coordinates from y = 0 correspond to "up".
        /// </summary>
        PillarCollider(std::vector<float> heightMap, std::vector<float> depthMap, float width, sf::Vector2f origin);

        vector<FloatRect> intersects(const FloatRect& collidingRect) const;


        std::vector<Vector2f> findSurfacePoints(const sf::Transform& T, const sf::FloatRect& rect) const;

        /// <summary>
        /// Checks for collision between a <see cref="FloatRect"/> and this <see cref="PillarCollider"/>.
        /// Returns a list of <see cref="FloatRect"/> objects l such that l[i] corresponds to a pillar that 
        /// the input <see cref="FloatRect"/> has an intersection with. 
        /// l[0] corresponds to the first pillar where there is an intersection, and l[l.size()-1]
        /// corresponds to the last pillar.
        /// </summary>
        /// <param name="collidingRect"></param>
        /// <returns></returns>
        vector<FloatRect> intersectsPillars(const sf::FloatRect& collidingRect) const;

        /// <summary>
        /// Checks for collision between a line and this <see cref="PillarCollider"/>. If it finds one, 
        /// output.second is a value t such that line.pointAt(t) is the earliest point along that line where a collision
        /// was found.
        /// </summary>
        /// <param name="line"></param>
        /// <returns></returns>
        std::pair<bool, float> intersects(const CustomUtilities::Segment& line) const;

        /// <summary>
        /// Gets a list of segments such for each segment s, s.p() and s.q() are points along this collider that lie along or within the Polygon.
        /// </summary>
        /// <param name="shape"></param>
        /// <returns></returns>
        std::vector<CustomUtilities::Segment> intersects(Polygon& shape) const;

        /// <summary>
        /// Checks if this collider contains a specified point.
        /// </summary>
        bool contains(sf::Vector2f& v, const sf::Transform& localToWorld) const;

        VertexArray toOutline() const;

        /// <summary>
        /// Returns a <see cref="VertexArray"/> that represents the outline of this collider
        /// as a polygon.
        /// </summary>
        VertexArray toPhysicalOutline() const;

        void draw(const sf::Transform& info, sf::RenderTarget& target, sf::RenderStates states, const sf::Color& color, bool debug) const;

        void origin(const sf::Vector2f& newOrigin);
        sf::Vector2f origin() const;

        static PillarCollider uniformHeight(std::vector<float> depthMap, float width, float height, sf::Vector2f position);
        static PillarCollider uniformDepth(std::vector<float> heightMap, float width, float depth, sf::Vector2f position);

    };
}

