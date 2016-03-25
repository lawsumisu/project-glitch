#pragma once

#include <SFML/Graphics/Rect.hpp>
#include "RectUtility.h"
#include "Collision.h"
#include "PillarCollider.h"

using namespace sf;
using namespace CustomUtilities;

namespace Physics {

    class SensorCollider {
    private:
        FloatRect ceiling, ground, left, right;
        Vector2f center;
    public:
        SensorCollider(Vector2f& center, Vector2f& horizontalDimensions, Vector2f& verticalDimensions);
        Collision collides(PillarCollider& collider);
    };
}

using namespace Physics;

SensorCollider::SensorCollider(Vector2f& center, Vector2f& horizontalDimensions, Vector2f& verticalDimensions) {
    this->center = center;
    ceiling = RectUtility::construct(center - Vector2f(0, verticalDimensions.y/2), verticalDimensions);
    ground = RectUtility::construct(center + Vector2f(0, verticalDimensions.y/2), verticalDimensions);
    right = RectUtility::construct(center + Vector2f(horizontalDimensions.x / 2, 0), horizontalDimensions);
    left = RectUtility::construct(center - Vector2f(horizontalDimensions.x / 2, 0), horizontalDimensions);
}

Collision SensorCollider::collides(PillarCollider& collider) {
    vector<FloatRect> groundCollisions = collider.intersects(ground);
    vector<FloatRect> ceilingCollisions = collider.intersects(ceiling);
    vector<FloatRect> leftCollisions = collider.intersects(left);
    vector<FloatRect> rightCollisions = collider.intersects(right);

    return Collision(groundCollisions, ceilingCollisions, leftCollisions, rightCollisions, collider.width);
}

