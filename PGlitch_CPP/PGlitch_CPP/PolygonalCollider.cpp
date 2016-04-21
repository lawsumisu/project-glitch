#include "stdafx.h"
#include "PolygonalCollider.h"
#include "LineUtility.h"
#include "RectUtility.h"
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include "GameInfo.h"

using namespace sf;
using namespace std;
using namespace CustomUtilities;

PolygonalCollider::PolygonalCollider(vector<Vector2f> points) {
    this->points = points;
    this->points.push_back(points[0]);

    float minX = points[0].x, maxX = points[0].x, minY = points[0].y, maxY = points[0].y;
    for (size_t i = 1; i < points.size(); ++i) {
        minX = min(minX, points[i].x);
        maxX = max(maxX, points[i].x);
        minY = min(minY, points[i].y);
        maxY = max(maxY, points[i].y);
    }
    maxBounds = FloatRect(minX, minY, maxX - minX, maxY - minY);

}

PolygonalCollider::PolygonalCollider(FloatRect rect) :
    PolygonalCollider({ Vector2f(rect.left, rect.top), Vector2f(rect.left + rect.width, rect.top),
        Vector2f(rect.left + rect.width, rect.top + rect.height), Vector2f(rect.left, rect.top + rect.height) }) {}

Vector2f PolygonalCollider::applyInfoToPoint(const sf::Vector2f& p, const PolygonalColliderInfo& info) const {
    float c = cosf(info.angle());
    float s = sinf(info.angle());
    Vector2f r = info.rotationalOffset();
    float x = c * (p.x - r.x) - s * (p.y - r.y) + r.x;
    float y = s * (p.x - r.x) + c * (p.y - r.y) + r.y;

    return Vector2f(x, y) + info.offset();
}
FloatRect PolygonalCollider::applyInfoToBounds(const PolygonalColliderInfo& info) const {
    Vector2f p1 = applyInfoToPoint(Vector2f(maxBounds.left, maxBounds.top), info);
    Vector2f p2 = applyInfoToPoint(Vector2f(maxBounds.left + maxBounds.width, maxBounds.top), info);
    Vector2f p3 = applyInfoToPoint(Vector2f(maxBounds.left + maxBounds.width, maxBounds.top + maxBounds.height), info);
    Vector2f p4 = applyInfoToPoint(Vector2f(maxBounds.left, maxBounds.top + maxBounds.height), info);

    float minX = p1.x, maxX = p1.x, minY = p1.y, maxY = p1.y;
    vector<Vector2f> boundPoints = { p2, p3, p4 };
    for (size_t i = 0; i < boundPoints.size(); ++i) {
        minX = min(minX, boundPoints[i].x);
        maxX = max(maxX, boundPoints[i].x);
        minY = min(minY, boundPoints[i].y);
        maxY = max(maxY, boundPoints[i].y);
    }

    return FloatRect(minX, minY, maxX - minX, maxY - minY);
}

pair<bool, Vector2f> PolygonalCollider::intersects(const PolygonalColliderInfo& info, const FloatRect& rect,
    CollisionDirection direction) const {

    //TODO update maxBounds with info.
    FloatRect effectiveMaxBounds = maxBounds;

    //Fail early if rect isn't even within max bounds.
    if (!effectiveMaxBounds.intersects(rect)) return{ false, Vector2f() };
    else {
        //Instead of checking intersects with rect, use maxbounds with rect to create a rect
        //whose intersects with this collider will provide the maximum location to translate to.
        FloatRect effectiveRect;
        if (direction == CollisionDirection::UP || direction == CollisionDirection::DOWN) {
            effectiveRect = FloatRect(rect.left, effectiveMaxBounds.top, rect.width, effectiveMaxBounds.height);
        }
        else effectiveRect = FloatRect(effectiveMaxBounds.left, rect.top, effectiveMaxBounds.width, rect.height);

        bool foundIntersection = false;
        Vector2f bestIntersection;

        for (size_t i = 0; i < points.size() - 1; ++i) {
            Line edge = Line(applyInfoToPoint(points[i], info), applyInfoToPoint(points[i + 1], info));

            vector<Vector2f> interiorPoints = edge.findInteriorPoints(effectiveRect);
            vector<Vector2f> intersections = edge.findAllIntersections(effectiveRect);
            intersections.insert(intersections.end(), interiorPoints.begin(), interiorPoints.end());
            for (size_t j = 0; j < intersections.size(); ++j) {
                if (!foundIntersection ||
                    (direction == CollisionDirection::UP && intersections[j].y < bestIntersection.y) ||
                    (direction == CollisionDirection::DOWN && intersections[j].y > bestIntersection.y) ||
                    (direction == CollisionDirection::LEFT && intersections[j].x < bestIntersection.x) ||
                    (direction == CollisionDirection::RIGHT && intersections[j].x > bestIntersection.x)) {
                    bestIntersection = intersections[j];
                    foundIntersection = true;

                    
                }
            }
        }
        Vector2f newOrigin = Vector2f(rect.left, rect.top);
        if (direction == CollisionDirection::UP) {
            newOrigin.y += bestIntersection.y - (rect.top + rect.height);
        }
        else if (direction == CollisionDirection::DOWN) {
            newOrigin.y = bestIntersection.y;
        }
        else if (direction == CollisionDirection::LEFT) {
            newOrigin.x += bestIntersection.x - (rect.left + rect.width);
        }
        else if (direction == CollisionDirection::RIGHT) {
            newOrigin.x = bestIntersection.x;
        }

        return{ foundIntersection, newOrigin};
    }
}

void PolygonalCollider::draw(const PolygonalColliderInfo& info, RenderTarget& target, RenderStates states, const Color& color) const {
    float ppu = GameInfo::pixelsPerUnit;
    VertexArray vertices = VertexArray(sf::LinesStrip, points.size());
    for (size_t i = 0; i < points.size(); ++i) {
        Vertex v = Vertex(applyInfoToPoint(points[i], info)*ppu, color);
        vertices[i] = v;
    }
    target.draw(vertices, states);
    CustomUtilities::draw(applyInfoToBounds(info) * ppu, Color::Red, target, states);
}