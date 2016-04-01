#include "stdafx.h"
#include "Collision.h"
#include "MathUtility.h"

using namespace Physics;

Collision::Collision(FloatRect& G, FloatRect& C, FloatRect& L, FloatRect& R, float angle) {
    _ground = RectUtility::copy(G);
    _ceiling = RectUtility::copy(C);
    _left = RectUtility::copy(L);
    _right = RectUtility::copy(R);
    _groundAngle = angle;
}
Collision::Collision(vector<FloatRect>& groundCollisions, vector<FloatRect>& ceilingCollisions,
    vector<FloatRect>& leftCollisions, vector<FloatRect>& rightCollisions) {

    _ground = findNearestCollision(groundCollisions, WallType::GROUND);
    _ceiling = findNearestCollision(ceilingCollisions, WallType::CEILING);
    _left = findNearestCollision(leftCollisions, WallType::LEFT);
    _right = findNearestCollision(rightCollisions, WallType::RIGHT);

    _groundAngle = calculateAngle(groundCollisions);

}

float Collision::calculateAngle(vector<FloatRect>& groundCollisions) {
    if (groundCollisions.size() <= 1) return 0;

    float minHeight = groundCollisions[0].top;
    float maxHeight = groundCollisions[0].top;

    for (size_t i = 0; i < groundCollisions.size(); ++i) {
        minHeight = min(minHeight, groundCollisions[i].top);
        maxHeight = max(maxHeight, groundCollisions[i].top);
    }
    // If the min and max heights are the same, then the terrain is flat.
    if (minHeight == maxHeight) return 0;

    vector<Vector2f> points = {};
    int n = 10;
    size_t last = groundCollisions.size() - 1;
    float fullWidth = groundCollisions[last].left + groundCollisions[last].width - 
        groundCollisions[0].left;
    float incrementalWidth = fullWidth / (n - 1);
    for (size_t i = 0; i < groundCollisions.size(); ++i) {
        for (float k = 0; k <= groundCollisions[i].width; k += incrementalWidth) {
            points.push_back(Vector2f(groundCollisions[i].left + k, groundCollisions[i].top));
        }
    }
    // Get the arctangent of the slope to get angle. 
    // Have to flip the sign due to game's y-coordinates having the opposite convention of
    // normal cartesian coordinates.
    return atan(-MathUtility::linearRegression(points).y);
}
FloatRect Collision::findNearestCollision(vector<FloatRect>& collisions, WallType type) {
    if (collisions.size() == 0) return FloatRect();

    FloatRect nearest = collisions[0];
    for (FloatRect& collision : collisions) {
        switch (type) {
        case WallType::CEILING:
            //Look for lowest ceiling
            if (collision.top + collision.height > nearest.top + nearest.height) nearest = collision;
            break;
        case WallType::GROUND:
            // Look for highest ground
            if (collision.top < nearest.top) nearest = collision;
            break;
        case WallType::LEFT:
            // Look for right-most left side
            if (collision.left + collision.width > nearest.left + nearest.width) nearest = collision;
            break;
        case WallType::RIGHT:
            // Look for left-most right side
            if (collision.left < nearest.left) nearest = collision;
            break;
        default:
            // Should never get here.
            break;
        }
    }
    return RectUtility::copy(nearest);
}

Collision Collision::reduce(Collision& otherCollision) const {
    FloatRect G = _ground;
    FloatRect C = _ceiling;
    FloatRect L = _left;
    FloatRect R = _right;
    float angle = _groundAngle;

    if (!withGround() || (otherCollision.withGround() && otherCollision.ground() < ground())) {
        G = otherCollision._ground;
        angle = otherCollision.groundAngle();
    }
    if (!withCeiling() || (otherCollision.withCeiling() && otherCollision.ceiling() > ceiling())) {
        C = otherCollision._ceiling;
    }
    if (!withLeft() || (otherCollision.withLeft() && otherCollision.left() > left())) {
        L = otherCollision._left;
    }
    if (!withRight() || (otherCollision.withRight() && otherCollision.right() < right())) {
        R = otherCollision._right;
    }

    return Collision(G, C, L, R, angle);
}

std::string Collision::toString() const {
    string na = "N/A";
    std::stringstream ss;
    ss << "Collision: ground(";
    if (withGround()) ss << ground();
    else ss << na;
    ss << "), ceiling(";
    if (withCeiling()) ss << ceiling();
    else ss << na;
    ss << "), left(";
    if (withLeft()) ss << left();
    else ss << na;
    ss << "), right(";
    if (withRight()) ss << right();
    else ss << na;
    ss << "), angle(" << (withGround() ? _groundAngle : 0.f) << ")";
    return ss.str();
}