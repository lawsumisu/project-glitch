#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <vector>
#include "RectUtility.h"

using namespace sf;
using namespace std;
using namespace CustomUtilities;

namespace Physics {
    class Collision {
    private:
        enum class WallType { LEFT, RIGHT, CEILING, GROUND };
        float _groundAngle;
        FloatRect _ground, _ceiling, _left, _right;
        FloatRect max(vector<FloatRect>& collisions, WallType type);
        float calculateAngle(vector<FloatRect>& groundCollisions, float width);
    public:
        Collision(vector<FloatRect>& groundCollisions, vector<FloatRect>& ceilingCollisions,
            vector<FloatRect>& leftCollisions, vector<FloatRect>& rightCollisions, float width);

        float groundAngle() const { return _groundAngle; }
        FloatRect ground() const { return RectUtility::copy(_ground); }
        FloatRect ceiling() const { return RectUtility::copy(_ground); }
        FloatRect left() const { return RectUtility::copy(_ground); }
        FloatRect right() const { return RectUtility::copy(_ground); }
    };
}

using namespace Physics;

Collision::Collision(vector<FloatRect>& groundCollisions, vector<FloatRect>& ceilingCollisions,
    vector<FloatRect>& leftCollisions, vector<FloatRect>& rightCollisions, float width) {

    _ground = max(groundCollisions, WallType::GROUND);
    _ceiling = max(ceilingCollisions, WallType::CEILING);
    _left = max(leftCollisions, WallType::LEFT);
    _right = max(rightCollisions, WallType::RIGHT);

    _groundAngle = calculateAngle(groundCollisions, width);
    
    // If the ground angle is over pi/2, then the incline is in the opposite direction, so
    // calculated angle must be flipped.
    const float pi = 4 * atan(1);
    if (_groundAngle > pi / 2) _groundAngle -= pi;

}

float Collision::calculateAngle(vector<FloatRect>& groundCollisions, float width) {
    if (groundCollisions.size() == 0) return 0;
    // ((X_t*X)^-1)*X_t*Y = A
    float x_sum = 0, y_sum = 0, xx_sum = 0, xy_sum = 0;
    for (int i = 0; i < groundCollisions.size(); ++i) {
        float x = i*width;
        float y = groundCollisions[i].top;
        x_sum += x;
        xx_sum += x*x;
        xy_sum += x*y;
        y_sum += y;
    }

    float determinant = 1.f / (groundCollisions.size()*xx_sum - x_sum*x_sum);
    float m = (-x_sum*y_sum + groundCollisions.size()*xy_sum) * determinant;
    return atan(determinant);
}
FloatRect Collision::max(vector<FloatRect>& collisions, WallType type) {
    if (collisions.size() == 0) return FloatRect();

    FloatRect max = collisions[0];
    for (FloatRect& collision : collisions) {
        switch (type) {
        case WallType::CEILING:
            //Look for lowest ceiling
            if (collision.top + collision.height > max.top + max.height) max = collision;
            break;
        case WallType::GROUND:
            // Look for highest ground
            if (collision.top < max.top) max = collision;
            break;
        case WallType::LEFT:
            // Look for right-most right side
            if (collision.left + collision.width > max.left + max.left) max = collision;
            break;
        case WallType::RIGHT:
            // Look for left-most left side
            if (collision.left < max.left) max = collision;
            break;
        default:
            // Should never get here.
            break;
        }
    }
    return RectUtility::copy(max);
}
