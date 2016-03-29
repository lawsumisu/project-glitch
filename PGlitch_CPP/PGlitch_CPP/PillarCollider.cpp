#include "stdafx.h"
#include "PillarCollider.h"

using namespace Physics;

PillarCollider::PillarCollider(std::vector<float> heightMap, vector<float> depthMap, float width, Vector2f position) {
    this->_width = width;
    this->origin = position;
    maxHeight = heightMap[0];
    maxDepth = depthMap[0];
    size = heightMap.size();
    pillars = {};
    //Create mask rectangles.
    for (size_t i = 0U; i < size; ++i) {
        float length = depthMap[i] - heightMap[i];
        pillars.push_back(FloatRect(position.x + i*width, position.y + heightMap[i], width, length));
        maxHeight = min(heightMap[i], maxHeight);
        maxDepth = max(depthMap[i], maxDepth);
    }

    maxBounds = FloatRect(origin.x, origin.y + maxHeight, _width*size, maxDepth - maxHeight);
}

VertexArray PillarCollider::toOutline() {
    VertexArray output = VertexArray(sf::LinesStrip, 4 * size+1);
    for (size_t i = 0U, j = size - 1; i < size; ++i, --j) {
        size_t ii = i * 2;
        size_t jj = output.getVertexCount() - 2 - j * 2;
        output[ii].position = Vector2f(pillars[i].left, pillars[i].top);
        output[ii + 1].position = Vector2f(pillars[i].left + _width, pillars[i].top);
        output[jj].position = Vector2f(pillars[j].left, pillars[j].top + pillars[j].height);
        output[jj - 1].position = Vector2f(pillars[j].left + _width,
            pillars[j].top + pillars[j].height);
    }
    output[output.getVertexCount() - 1] = output[0];
    return output;
}

vector<FloatRect> PillarCollider::intersects(FloatRect& collidingRect) const {
    if (!maxBounds.intersects(collidingRect)) return{};

    //First need to determine span of indices to check.
    float xMin = collidingRect.left - origin.x;
    float xMax = xMin + collidingRect.width;

    // Check that bounds are legitimate for intersection.
    if ((xMin < 0 && xMax <= 0) || (xMax < 0)) return{};

    size_t iMin = (size_t)max(0.f, floorf(xMin / _width));
    size_t iMax = min(size-1, (size_t)floorf(xMax / _width));
    if (floorf(xMax / _width) == xMax / _width) --iMax;

    vector<FloatRect> output = {};
    for (size_t i = iMin; i <= iMax; ++i) {
        FloatRect R = FloatRect();
        collidingRect.intersects(pillars[i], R);
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
