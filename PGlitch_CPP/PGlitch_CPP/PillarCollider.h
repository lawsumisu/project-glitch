#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>
#include <iostream>

using namespace sf;
using namespace std;

namespace Physics {
    
    /// <summary>
    /// Represents a collider whose shape is that of a series of same-width rectangles.
    /// </summary>
    class PillarCollider {
    private:
        float _width, maxHeight, maxDepth;
        Vector2f origin;
        size_t size;
        vector<FloatRect> maskRects;
      
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
        vector<FloatRect> intersects(FloatRect collidingRect);
        float width() { return _width; }
        VertexArray toOutline();
        static PillarCollider uniformHeight(vector<float> depthMap, float width, float height, Vector2f position);
        static PillarCollider uniformDepth(vector<float> heightMap, float width, float height, Vector2f position);

    };
}

using namespace Physics;
PillarCollider::PillarCollider(std::vector<float> heightMap, vector<float> depthMap, float width, Vector2f position) {
    this->_width = width;
    this->origin = position;
    maxHeight = heightMap[0];
    maxDepth = depthMap[0];
    size = heightMap.size();
    maskRects = {};
    //Create mask rectangles.
    for (size_t i = 0U; i < size; ++i) {
        float length = depthMap[i] - heightMap[i];
        maskRects.push_back(FloatRect(position.x + i*width, position.y + heightMap[i], width, length));
        maxHeight = min(heightMap[i], maxHeight);
        maxDepth = max(depthMap[i], maxDepth);
    }
}

VertexArray PillarCollider::toOutline() {
    VertexArray output = VertexArray(sf::LinesStrip, 4*size);
    for (size_t i = 0U, j = size-1; i < size; ++i,--j) {
        size_t ii = i * 2;
        size_t jj = output.getVertexCount() - 1 - j * 2;
        output[ii].position = Vector2f(maskRects[i].left, maskRects[i].top);
        output[ii + 1].position = Vector2f(maskRects[i].left + _width, maskRects[i].top);
        output[jj].position = Vector2f(maskRects[j].left, maskRects[j].top+maskRects[j].height);
        output[jj - 1].position = Vector2f(maskRects[j].left + _width, 
            maskRects[j].top + maskRects[j].height);
    }
    return output;
}

vector<FloatRect> PillarCollider::intersects(FloatRect collidingRect) {
    bool intersectionCondition1 = collidingRect.top < origin.y + maxDepth &&
        collidingRect.top > origin.y + maxHeight;
    bool intersectionCondition2 = collidingRect.top + collidingRect.height < origin.y + maxDepth &&
        collidingRect.top + collidingRect.height > origin.y + maxHeight;
    if (!(intersectionCondition1 || intersectionCondition2)) return{};

    //First need to determine span of indices to check.
    float xMin = collidingRect.left - origin.x;
    float xMax = xMin + collidingRect.width;

    // Check that bounds are legitimate for intersection.
    if ((xMin < 0 && xMax <= 0) || (xMax < 0)) return{};

    size_t iMin = (size_t) max(0.f, floorf(xMin / _width));
    size_t iMax = min(size, (size_t) floorf(xMax / _width));
    if (floorf(xMax / _width) == xMax / _width) --iMax;

    vector<FloatRect> output = {};
    for (size_t i = iMin; i <= iMax; ++i) {
        FloatRect R = FloatRect();
        collidingRect.intersects(maskRects[i], R);
        output.push_back(R);
    }
    return output;
}

PillarCollider PillarCollider::uniformDepth(vector<float> heightMap, float width, float depth, Vector2f position) {
    return PillarCollider(heightMap, vector<float>(heightMap.size(), depth), width, position);
}

PillarCollider PillarCollider::uniformHeight(vector<float> depthMap, float width, float height, Vector2f position) {
    return PillarCollider(vector<float>(depthMap.size(), height), depthMap, width, position);
}
