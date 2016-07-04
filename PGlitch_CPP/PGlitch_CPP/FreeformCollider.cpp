#include "stdafx.h"
#include "FreeformCollider.h"
#include "LineUtility.h"
#include "RectUtility.h"
#include "VectorUtility.h"
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include "GameInfo.h"
#include <unordered_set>
#include <functional>
#include <time.h>

using namespace sf;
using namespace std;
using namespace CustomUtilities;

FreeformCollider::FreeformCollider(vector<Vector2f> points) :sorter({}), colliderShape(points) {
    this->points = points;
    //this->points.push_back(points[0]);

    float minX = points[0].x, maxX = points[0].x, minY = points[0].y, maxY = points[0].y;
    float mbMinX = points[0].x, mbMaxX = points[0].x, mbMinY = points[0].y, mbMaxY = points[0].y;
    for (size_t i = 0; i < points.size(); ++i) {
        size_t idx = (i + 1) % points.size();
        minX = min(minX, points[idx].x);
        maxX = max(maxX, points[idx].x);
        minY = min(minY, points[idx].y);
        maxY = max(maxY, points[idx].y);

        //Prepare inner bounds if necessary.
        mbMinX = min(mbMinX, points[idx].x);
        mbMaxX = max(mbMaxX, points[idx].x);
        mbMinY = min(mbMinY, points[idx].y);
        mbMaxY = max(mbMaxY, points[idx].y);

        if ((i >= 1 && i % 10 == 0) || i == points.size()-1) {
            boundsList.push_back(FloatRect(mbMinX, mbMinY, mbMaxX - mbMinX, mbMaxY - mbMinY));
            boundIndices.push_back(i);
            if (i < points.size() - 1) {
                mbMinX = points[i].x;
                mbMaxX = points[i].x;
                mbMinY = points[i].y;
                mbMaxY = points[i].y;
            }
        }
    }
    maxBounds = FloatRect(minX, minY, maxX - minX, maxY - minY);
    quadtree = Quadtree(maxBounds, 0);
    for (size_t i = 0; i < points.size(); ++i) {
        size_t j = (i + 1) % points.size();
        quadtree.insert(Segment(points[i], points[j]));
    }

    //Set up mini bounds.
    sorter = RectSorter(boundsList);

}

FreeformCollider::FreeformCollider(FloatRect rect) :
    FreeformCollider({ Vector2f(rect.left, rect.top), Vector2f(rect.left + rect.width, rect.top),
        Vector2f(rect.left + rect.width, rect.top + rect.height), Vector2f(rect.left, rect.top + rect.height) }) {}

vector<Segment> FreeformCollider::intersects(const Polygon& shape, const sf::Transform& T) const {
    vector<Segment> segments =  colliderShape.findOverlappingLines(shape.transform(T.getInverse()));
    for (size_t i = 0; i < segments.size(); ++i) segments[i] = segments[i].transform(T);
    return segments;
}

bool FreeformCollider::contains(Vector2f& v, const Transform& localToWorld) const {
    return colliderShape.containsPoint(localToWorld.getInverse().transformPoint(v));
}

void FreeformCollider::draw(const Transform& info, RenderTarget& target, RenderStates states, const Color& color, bool debug) const {
    float ppu = GameInfo::pixelsPerUnit;
    VertexArray vertices = VertexArray(sf::LinesStrip, points.size());
    for (size_t i = 0; i < points.size(); ++i) {
        Vertex v = Vertex(info.transformPoint(points[i])*ppu, color);
        vertices[i] = v;
    }
    vertices.append(vertices[0]);
    target.draw(vertices, states);
    if (debug) {
        /*CustomUtilities::draw(maxBounds, Color::Red, target, states);
        for (const auto& bbox : boundsList) {
            CustomUtilities::draw(info.transformRect(bbox), Color::Green, target, states);
        }*/
        //quadtree.draw(target, states);
        colliderShape.drawTree(target, states);
    }
    
}