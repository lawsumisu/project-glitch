#include "stdafx.h"
#include "PillarCollider.h"
#include "RectUtility.h"
#include "MathUtility.h"
#include "VectorUtility.h"
#include "GameInfo.h"

using namespace Physics;
using namespace CustomUtilities;

PillarCollider::PillarCollider(std::vector<float> heightMap, vector<float> depthMap, float width, Vector2f position) {
    this->_width = width;
    _origin = position;
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

    maxBounds = FloatRect(_origin.x, _origin.y + maxHeight, _width*size, maxDepth - maxHeight);
}

VertexArray PillarCollider::toOutline() const {
    float ppu = GameInfo::pixelsPerUnit;
    VertexArray output = VertexArray(sf::LinesStrip, 4 * size+1);
    for (size_t i = 0U, j = size - 1; i < size; ++i, --j) {
        size_t ii = i * 2;
        size_t jj = output.getVertexCount() - 2 - j * 2;
        output[ii].position = Vector2f(pillars[i].left, pillars[i].top)*ppu;
        output[ii + 1].position = Vector2f(pillars[i].left + _width, pillars[i].top)*ppu;
        output[jj].position = Vector2f(pillars[j].left, pillars[j].top + pillars[j].height)*ppu;
        output[jj - 1].position = Vector2f(pillars[j].left + _width,
            pillars[j].top + pillars[j].height)*ppu;
    }
    output[output.getVertexCount() - 1] = output[0];
    return output;
}

VertexArray PillarCollider::toPhysicalOutline() const {
    float ppu = GameInfo::pixelsPerUnit;
    VertexArray output = VertexArray(sf::LinesStrip, 0);
    bool drawUp = true;
    for (size_t i = 0U; i < size - 1; ++i) {
        if (drawUp) {
            output.append(Vertex(Vector2f(pillars[i].left, pillars[i].top)*ppu));
            if (pillars[i+1].top >= pillars[i].top) {
                drawUp = false;
                output.append(Vertex(Vector2f(pillars[i].left+pillars[i].width, pillars[i].top)*ppu));
            }
        }
        else {
            if (pillars[i+1].top <= pillars[i].top) {
                drawUp = true;
                output.append(Vertex(Vector2f(pillars[i].left, pillars[i].top)*ppu));
            }
            output.append(Vertex(Vector2f(pillars[i].left + pillars[i].width, pillars[i].top)*ppu));
        }
    }
    FloatRect p = pillars[size - 1];
    if (drawUp) output.append(Vertex(Vector2f(p.left, p.top)));
    output.append(Vertex(Vector2f(p.left + p.width, p.top)*ppu));
    output.append(Vertex(Vector2f(p.left + p.width, p.top + p.height)*ppu));
    output.append(Vertex(Vector2f(pillars[0].left, pillars[0].top + pillars[0].height)*ppu));
    output.append(output[0]);

    for (size_t i = 0; i < output.getVertexCount(); i++) output[i].color = Color::Green;
    return output;
}

void PillarCollider::draw(const Transform& T, RenderTarget& target, RenderStates states, const Color& color, bool debug) const {
    for (auto& pillar : pillars) {
        if (debug) {
            CustomUtilities::draw(pillar, Color::Red, target, states);
        }
        CustomUtilities::draw(T.transformRect(pillar), color, target, states);        
    }
    
}

bool PillarCollider::contains(Vector2f& v, const Transform& localToWorld) const {
    Vector2f w = localToWorld.getInverse().transformPoint(v);
    int i = (int)((w.x - _origin.x) / _width);
    if (i < 0 || i >= pillars.size()) return false;
    else return pillars[i].contains(w);
}
vector<Vector2f> PillarCollider::findSurfacePoints(const Transform& T, const FloatRect& rect) const{
    FloatRect effectiveRect = T.getInverse().transformRect(rect);

    if (!maxBounds.intersects(effectiveRect)) return{};

    //First need to determine span of indices to check.
    float xMin = effectiveRect.left - _origin.x;
    float xMax = xMin + effectiveRect.width;

    // Check that bounds are legitimate for intersection.
    if ((xMin < 0 && xMax <= 0) || (xMax < 0)) return{};

    size_t iMin = (size_t)max(0.f, floorf(xMin / _width));
    size_t iMax = min(size - 1, (size_t)floorf(xMax / _width));

    vector<Vector2f> output;
    for (size_t i = iMin; i <= iMax; ++i) {
        if (effectiveRect.intersects(pillars[i])) {
            output.push_back(T.transformPoint(Vector2f(pillars[i].left, pillars[i].top)));
            output.push_back(T.transformPoint(Vector2f(pillars[i].left + pillars[i].width, pillars[i].top)));
        }
    }
    return output;
}

pair<int, int> PillarCollider::getBoundIndices(sf::FloatRect& bounds) const {
    if (!maxBounds.intersects(bounds)) return{ -1,-1 };
    //First need to determine span of indices to check.
    float xMin = bounds.left - _origin.x;
    float xMax = xMin + bounds.width;

    // Check that bounds are legitimate for intersection.
    //if ((xMin < 0 && xMax <= 0) || (xMax < 0)) return{};

    size_t iMin = (size_t)max(0.f, floorf(xMin / _width));
    size_t iMax = min(size - 1, (size_t)floorf(xMax / _width));

    return{ iMin, iMax };
}

vector<Segment> PillarCollider::intersects(Polygon& shape) const {
    pair<int, int> indices = getBoundIndices(shape.bounds());

    if (indices.first == -1) return{};
    else {
        vector<Segment> output = {};
        for (int i = indices.first; i <= indices.second; ++i) {
            FloatRect R = FloatRect();
            vector<Segment> innerLines = shape.findInternalLines(pillars[i]);
            output.insert(output.end(), innerLines.begin(), innerLines.end());
        }
        return output;
    }
}

vector<FloatRect> PillarCollider::intersects(const FloatRect& collidingRect) const {
    pair<int, int> indices = getBoundIndices(FloatRect(collidingRect));
    if (indices.first == -1) return{};

    vector<FloatRect> output = {};
    for (int i = indices.first; i <= indices.second; ++i) {
        FloatRect R = FloatRect();
        collidingRect.intersects(pillars[i], R);
        if (area(R) != 0)output.push_back(R);
    }
    return output;
}

vector<FloatRect> PillarCollider::intersectsPillars(const FloatRect& collidingRect) const{
    if (!maxBounds.intersects(collidingRect)) return{};

    //First need to determine span of indices to check.
    float xMin = collidingRect.left - _origin.x;
    float xMax = xMin + collidingRect.width;

    // Check that bounds are legitimate for intersection.
    if ((xMin < 0 && xMax <= 0) || (xMax < 0)) return{};

    size_t iMin = (size_t)max(0.f, floorf(xMin / _width));
    size_t iMax = min(size - 1, (size_t)floorf(xMax / _width));

    vector<FloatRect> output = {};
    for (size_t i = iMin; i <= iMax; ++i) {
        if (collidingRect.intersects(pillars[i])) output.push_back(FloatRect(pillars[i]));

    }
    return output;
}
pair<bool, float> PillarCollider::intersects(const Segment& line) const{
    if (!line.intersects(maxBounds).first) return{ false, 0.f };
    bool foundIntersect = false;
    float t = 1;
    for (const FloatRect& R : pillars) {
        pair<bool, float> intersection = line.intersects(R);
        if (intersection.first) {
            foundIntersect = true;
            t = min(t, intersection.second);
        }
    }
    return{ foundIntersect, t };
}

void PillarCollider::origin(const Vector2f& newOrigin) {
    Vector2f diff = newOrigin - _origin;
    _origin = newOrigin;

    maxBounds.left += diff.x;
    maxBounds.top += diff.y;

    for (FloatRect& R : pillars) {
        R.left += diff.x;
        R.top += diff.y;
    }
}

Vector2f PillarCollider::origin() const {
    return Vector2f(_origin);
}

PillarCollider PillarCollider::uniformDepth(vector<float> heightMap, float width, float depth, Vector2f position) {
    return PillarCollider(heightMap, vector<float>(heightMap.size(), depth), width, position);
}

PillarCollider PillarCollider::uniformHeight(vector<float> depthMap, float width, float height, Vector2f position) {
    return PillarCollider(vector<float>(depthMap.size(), height), depthMap, width, position);
}
