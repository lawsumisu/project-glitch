#include "stdafx.h"
#include "SensorCollider.h"
#include "VectorUtility.h"
#include "RectUtility.h"
#include "MathUtility.h"
#include "GameInfo.h"

using namespace Physics;

SensorCollider::SensorCollider(Vector2f& center, Vector2f& horizontalDimensions, Vector2f& verticalDimensions) {
    this->center = center;
    _ceiling = construct(center - Vector2f(0, verticalDimensions.y / 2), verticalDimensions);
    _ground = construct(center + Vector2f(0, verticalDimensions.y / 2), verticalDimensions);
    _right = construct(center + Vector2f(horizontalDimensions.x / 2, 0), horizontalDimensions);
    _left = construct(center - Vector2f(horizontalDimensions.x / 2, 0), horizontalDimensions);
}

SensorCollider::SensorCollider(Vector2f& center, Vector2f& size, Vector2f& horizontalDimensions, Vector2f& verticalDimensions) {
    this->center = center;
    _ceiling = construct(center - Vector2f(0, size.y / 2), verticalDimensions);
    _ground = construct(center + Vector2f(0, size.y / 2), verticalDimensions);
    _right = construct(center + Vector2f(horizontalDimensions.x / 2, 0), horizontalDimensions);
    _left = construct(center - Vector2f(horizontalDimensions.x / 2, 0), horizontalDimensions);
}
Collision SensorCollider::collides(const PillarCollider& collider) const {
    vector<FloatRect> groundCollisions = collider.intersectsPillars(_ground);
    vector<FloatRect> ceilingCollisions = collider.intersectsPillars(_ceiling);
    vector<FloatRect> leftCollisions = collider.intersectsPillars(_left);
    vector<FloatRect> rightCollisions = collider.intersectsPillars(_right);

    vector<FloatRect> reducedGroundCollisions = {};
    for (FloatRect& r : groundCollisions) {
        if (_ground.top < r.top) reducedGroundCollisions.push_back(r);
    }

    return Collision(reducedGroundCollisions, ceilingCollisions, leftCollisions, rightCollisions);
}

Collision SensorCollider::collides(const vector<PillarCollider>& colliders) const {
    if (colliders.size() == 1) return collides(colliders[0]);

    Collision C = collides(colliders[0]);
    for (size_t i = 1U; i < colliders.size(); ++i) {
        C = C.reduce(collides(colliders[i]));
    }
    return C;

}

Collision SensorCollider::collides(const Platform& platform) const {
    if (platform._type == PlatformType::SOLID) return collides(platform._pillars);
    else {
        vector<FloatRect> empty = {};
        vector<FloatRect> groundCollisions = platform._pillars.intersectsPillars(_ground);
        vector<FloatRect> reducedGroundCollisions = {};
        for (FloatRect& r : groundCollisions) {
            if (_ground.top < r.top) reducedGroundCollisions.push_back(r);
        }
        return Collision(reducedGroundCollisions, empty, empty, empty);
    }
}

pair<Collision, int> SensorCollider::collides(const vector<Platform>& platforms) const {
    if (platforms.size() == 0U) return{ Collision(), -1 };
    if (platforms.size() == 1) {
        Collision collision = collides(platforms[0]);
        if (collision.withGround()) return{ collision, 0 };
        else return{ collision, -1 };
    }

    Collision C = collides(platforms[0]);

    int index = -1;
    float groundHeight = 0;
    if (C.withGround()) {
        index = 0;
        groundHeight = C.ground();
    }
    for (size_t i = 1U; i < platforms.size(); ++i) {
        C = C.reduce(collides(platforms[i]));
        if (C.withGround()) {
            if (index == -1 || C.ground() != groundHeight) {
                groundHeight = C.ground();
                index = i;
            }
        }
    }
    return{ C, index };
}

Collision SensorCollider::collidesHorizontal(const PillarCollider& collider) const {
    vector<FloatRect> leftCollisions = collider.intersectsPillars(_left);
    vector<FloatRect> rightCollisions = collider.intersectsPillars(_right);
    vector<FloatRect> empty = {};

    return Collision(empty, empty, leftCollisions, rightCollisions);
}

pair<Collision, pair<int, int>> SensorCollider::collidesHorizontal(const vector<PillarCollider>& pillars) const {
    if (pillars.size() == 0) return{ Collision(), {-1,-1} };
    else if (pillars.size() == 1) {
        int l = -1, r = -1;
        Collision collision = collidesHorizontal(pillars[0]);
        if (collision.withLeft()) l = 0;
        if (collision.withRight()) r = 0;
        return{ collision, {l, r} };
     }
    else {
        Collision collision = collides(pillars[0]);

        int l = -1, r = -1;
        float leftValue = 0, rightValue = 0;
        for (size_t i = 1U; i < pillars.size(); ++i) {
            collision = collision.reduce(collidesHorizontal(pillars[i]));
            if (collision.left()) {
                if (l == -1 || collision.left() != leftValue) {
                    l = i;
                    leftValue = collision.left();
                }
            }
            if (collision.right() && (r == -1 || collision.right() != rightValue)) {
                r = i;
                rightValue = collision.right();
            }
        }
        return{ collision, {l, r} };
    }
}

Collision SensorCollider::collidesVertical(const PillarCollider& pillar) const {
    vector<FloatRect> groundCollisions = pillar.intersectsPillars(_ground);
    vector<FloatRect> ceilingCollisions = pillar.intersectsPillars(_ceiling);
    vector<FloatRect> empty = {};

    vector<FloatRect> reducedGroundCollisions = {};
    for (FloatRect& r : groundCollisions) {
        if (_ground.top < r.top) reducedGroundCollisions.push_back(r);
    }
    return Collision(reducedGroundCollisions, ceilingCollisions, empty, empty);
}

pair<Collision, pair<int, int>> SensorCollider::collidesVertical(const vector<PillarCollider>& pillars) const {
    if (pillars.size() == 0) return{ Collision(),{ -1,-1 } };
    else if (pillars.size() == 1) {
        int g = -1, c = -1;
        Collision collision = collidesVertical(pillars[0]);
        if (collision.withGround()) g = 0;
        if (collision.withCeiling()) c = 0;
        return{ collision,{ g, c } };
    }
    else {
        Collision collision = collides(pillars[0]);

        int g = -1, c = -1;
        float groundValue = 0, ceilingValue = 0;
        for (size_t i = 1U; i < pillars.size(); ++i) {
            collision = collision.reduce(collidesVertical(pillars[i]));
            if (collision.withGround() && (g == -1 || collision.ground() != groundValue)) {
                g = i;
                groundValue = collision.ground();
            }
            if (collision.withCeiling() && (c == -1 || collision.ceiling() != ceilingValue)) {
                c = i;
                ceilingValue = collision.ceiling();
            }
        }
        return{ collision, {g, c} };
    }
}

vector<pair<size_t, float>> SensorCollider::collides(const std::vector<Platform>& platforms, SensorType type) const {
    vector<pair<size_t, float>> output = {};
    size_t minIndex = 0;
    float nearestValue = 0;
    for (size_t i = 0; i < platforms.size(); ++i) {
        //Only collide with solid platforms for ceiling, left, and right collisions.
        if (type != SensorType::GROUND && platforms[i].type() != PlatformType::SOLID) continue;

        //Get potential collisions based on type.
        FloatRect R = _ground;
        if (type == SensorType::LEFT) R = _left;
        else if (type == SensorType::RIGHT)  R = _right;
        else if (type == SensorType::CEILING) R = _ceiling;
        vector<FloatRect> potentialCollisions = platforms[i]._pillars.intersectsPillars(R);
        if (potentialCollisions.size() == 0) continue;

        //Get collision value based on type.
        float collisionValue;
        if (type == SensorType::GROUND) {
            collisionValue = potentialCollisions[0].top;
            for (size_t j = 1; j < potentialCollisions.size(); ++j) {
                collisionValue = min(collisionValue, potentialCollisions[j].top);
            }
        }
        else if (type == SensorType::CEILING) {
            collisionValue = potentialCollisions[0].top + potentialCollisions[0].height;
            for (size_t j = 1; j < potentialCollisions.size(); ++j) {
                collisionValue = max(collisionValue, potentialCollisions[j].top + potentialCollisions[j].height);
            }
        }
        else if (type == SensorType::LEFT) {
            collisionValue = potentialCollisions[potentialCollisions.size() - 1].left +
                potentialCollisions[potentialCollisions.size() - 1].width;
        }
        else if (type == SensorType::RIGHT) {
            collisionValue = potentialCollisions[0].left;
        }        

        //Keep track of nearest collision.
        if (output.size() == 0) {
            nearestValue = collisionValue;
        }
        if (((type == SensorType::GROUND || type == SensorType::RIGHT) && collisionValue < nearestValue ||
            (type == SensorType::CEILING || type == SensorType::LEFT) && collisionValue > nearestValue)) {
            nearestValue = collisionValue;
            minIndex = i;
        }
        output.push_back({ i, collisionValue });
    }

    // Add nearest value to end of list if not empty.
    if (output.size() != 0) output.push_back({ minIndex, nearestValue });
    return output;
}

vector<pair<size_t, float>> SensorCollider::collidesGround(const vector<Platform>& platforms) const {
    return collides(platforms, SensorType::GROUND);
}

vector<pair<size_t, float>> SensorCollider::collidesCeiling(const vector<Platform>& platforms) const {
    return collides(platforms, SensorType::CEILING);
}

vector<pair<size_t, float>> SensorCollider::collidesLeft(const vector<Platform>& platforms) const {
    return collides(platforms, SensorType::LEFT);
}

vector<pair<size_t, float>> SensorCollider::collidesRight(const vector<Platform>& platforms) const {
    return collides(platforms, SensorType::RIGHT);
}

float SensorCollider::groundAngle(const Platform& platform) const {
    vector<FloatRect> groundCollisions = platform._pillars.intersectsPillars(_ground);

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
    return atan(-linearRegression(points).y);
}

vector<pair<size_t, float>> SensorCollider::collides(const std::vector<PlatformPtr>& platforms, SurfaceType type) const {
    vector<pair<size_t, float>> output = {};
    size_t minIndex = 0;
    float nearestValue = 0;
    FloatRect collisionRect = _ground;
    if (type == SurfaceType::CEILING) collisionRect = _ceiling;
    else if (type == SurfaceType::RIGHT) collisionRect = _right;
    else if (type == SurfaceType::LEFT) collisionRect = _left;

    for (size_t i = 0; i < platforms.size(); ++i) {
        //Only collide with solid platforms for ceiling, left, and right collisions.
        if (type != SurfaceType::GROUND && platforms[i]->type() != PlatformType::SOLID) {
            continue;
        }

        pair<bool, float> collision = platforms[i]->collides(collisionRect, type);
        if (collision.first) {
            if (output.size() == 0 ||
                ((type == SurfaceType::GROUND|| type == SurfaceType::RIGHT) && collision.second < nearestValue) ||
                (type == SurfaceType::CEILING || type == SurfaceType::LEFT && collision.second > nearestValue)) {
                nearestValue = collision.second;
                minIndex = i;
            }
            output.push_back({ i, collision.second });
        }
    }

    // Add nearest value to end of list if not empty.
    if (output.size() != 0) output.push_back({ minIndex, nearestValue });
    return output;
}

float SensorCollider::groundAngle(const PlatformPtr& platform) const {
    return platform->groundAngle(_ground);
}

void SensorCollider::setCenter(Vector2f newCenter) {
    Vector2f delta = newCenter - center;
    if (delta.x != 0 || delta.y != 0) {
        center = newCenter;
        _ground.left += delta.x;
        _ground.top += delta.y;
        _ceiling.left += delta.x;
        _ceiling.top += delta.y;
        _left.left += delta.x;
        _left.top += delta.y;
        _right.left += delta.x;
        _right.top += delta.y;
    }
}
void SensorCollider::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    float ppu = GameInfo::pixelsPerUnit;
    CustomUtilities::draw(ground(), Color::Green, target, states);
    CustomUtilities::draw(right(), Color::Red, target, states);
    CustomUtilities::draw(left(), Color::Blue, target, states);
    CustomUtilities::draw(ceiling(), Color::Cyan, target, states);
}