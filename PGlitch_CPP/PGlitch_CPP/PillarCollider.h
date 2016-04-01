#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

using namespace sf;
using namespace std;

namespace Physics {
    
    /// <summary>
    /// Represents a collider whose shape is that of a series of same-width rectangles or "pillars".
    /// </summary>
    class PillarCollider {
    private:
        float _width, maxHeight, maxDepth;
        Vector2f origin;
        size_t size;
        vector<FloatRect> pillars;
        FloatRect maxBounds;
      
    public:
        /// <summary>
        /// Initalizes a <see cref="MaskCollider"/> object with origin origin, whose shape is a series of rectangles
        /// all of width width. Within the collider, a rectangle r[i] has a length == depthMap[i] - heightMap[i].
        /// Requires that heightMap.size() == depthMap.size() and both be >= 1.
        /// Requires that depthMap[i] >= heightMap[i].
        /// *Follows the convention that positive y-coordinates from y = 0 correspond to "down" and 
        /// negative y-coordinates from y = 0 correspond to "up".
        /// </summary>
        PillarCollider(std::vector<float> heightMap, vector<float> depthMap, float width, Vector2f origin);
        vector<FloatRect> intersects(FloatRect& collidingRect) const;
        VertexArray toOutline() const;

        /// <summary>
        /// Returns a <see cref="VertexArray"/> that represents the outline of this collider
        /// as a polygon.
        /// </summary>
        VertexArray toPhysicalOutline() const;
        static PillarCollider uniformHeight(vector<float> depthMap, float width, float height, Vector2f position);
        static PillarCollider uniformDepth(vector<float> heightMap, float width, float depth, Vector2f position);

    };
}

