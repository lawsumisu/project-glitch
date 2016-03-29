#include "stdafx.h"
#include "Collision.h"

using namespace Physics;

Collision::Collision(FloatRect& G, FloatRect& C, FloatRect& L, FloatRect& R, float angle) {
    _ground = RectUtility::copy(G);
    _ceiling = RectUtility::copy(C);
    _left = RectUtility::copy(L);
    _right = RectUtility::copy(R);
    _groundAngle = angle;
}
Collision::Collision(vector<FloatRect>& groundCollisions, vector<FloatRect>& ceilingCollisions,
    vector<FloatRect>& leftCollisions, vector<FloatRect>& rightCollisions, float width) {

    _ground = findNearestCollision(groundCollisions, WallType::GROUND);
    _ceiling = findNearestCollision(ceilingCollisions, WallType::CEILING);
    _left = findNearestCollision(leftCollisions, WallType::LEFT);
    _right = findNearestCollision(rightCollisions, WallType::RIGHT);

    _groundAngle = calculateAngle(groundCollisions, width);

    // If the ground angle is over pi/2, then the incline is in the opposite direction, so
    // calculated angle must be flipped.
    const float pi = 4 * atanf(1);
    if (_groundAngle > pi / 2) _groundAngle -= pi;

}

float Collision::calculateAngle(vector<FloatRect>& groundCollisions, float width) {
    if (groundCollisions.size() <= 1) return 0;

    float minHeight = groundCollisions[0].top;
    float maxHeight = groundCollisions[0].top;

    for (size_t i = 0; i < groundCollisions.size(); ++i) {
        minHeight = min(minHeight, groundCollisions[i].top);
        maxHeight = max(maxHeight, groundCollisions[i].top);
    }
    // If the min and max heights are the same, then the terrain is flat.
    if (minHeight == maxHeight) return 0;

    // ((X_t*X)^-1)*X_t*Y = A
    float x_sum = 0, y_sum = 0, xx_sum = 0, xy_sum = 0;
    for (size_t i = 0; i < groundCollisions.size(); ++i) {
        if (groundCollisions[i].height * groundCollisions[i].width == 0) continue;
        float x = i*width;
        float y = groundCollisions[i].top;
        x_sum += x;
        xx_sum += x*x;
        xy_sum += x*y;
        y_sum += y;
    }

    float determinant = 1.f / (groundCollisions.size()*xx_sum - x_sum*x_sum);
    float m = (-x_sum*y_sum + groundCollisions.size()*xy_sum) * determinant;
    return atan(m);
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
    ss << ")";

    return ss.str();
}