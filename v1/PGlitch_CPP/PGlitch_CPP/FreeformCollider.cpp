#include "stdafx.h"
#include "FreeformCollider.h"
#include "Segment.h"
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

FreeformCollider::FreeformCollider(vector<Vector2f> points) : colliderShape(points) {}

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
    colliderShape.transform(info).draw(target, states);
    if (debug) {
        colliderShape.drawTree(target, states);
    }
    
}