#include "stdafx.h"
#include "RaycastCollider.h"
#include "VectorUtility.h"
#include "MathUtility.h"
#include "GameInfo.h"
#include <iostream>

using namespace Physics;
using namespace sf;

RaycastCollider::RaycastCollider(const Vector2f& origin, const Vector2f& size, float angle, size_t rayCount) {
    this->_angle = angle;
    this->size = size;
    this->rayCount = rayCount;

    setOrigin(origin);
}

std::pair<bool, float> RaycastCollider::collides(PillarCollider& pillar) const {
    bool foundIntersection = false;
    float t = 1;
    for (const Line& ray : rays) {
        pair<bool, float> intersection = pillar.intersects(ray);
        if (intersection.first) {
            foundIntersection = true;
            t = min(t, intersection.second);
        }
    }
    return{ foundIntersection, t };
}

void RaycastCollider::setOrigin(const Vector2f& newOrigin) {
    origin = newOrigin;

    constructRays();
}

void RaycastCollider::constructRays() {
    rays = {};
    //Create list of rays.
    Vector2f offset = toPoint(size.y / 2, _angle - pi / 2);
    for (size_t i = 0; i < rayCount; ++i) {
        Vector2f w = toPoint(size.y * i / (rayCount - 1), _angle - pi / 2);
        rays.push_back(Line(origin - offset + w, _angle, size.x / 2));
    }
}

void RaycastCollider::angle(float newAngle) {
    _angle = newAngle;
    constructRays();
}

float RaycastCollider::angle() const {
    return _angle;
}
void RaycastCollider::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (const Line& ray : rays) {
        (ray*GameInfo::pixelsPerUnit).draw(Color::Red, target, states);
    }
}