#include "stdafx.h"
#include "SensorCollider.h"
#include "VectorUtility.h"
#include "RectUtility.h"
#include "GameInfo.h"

using namespace Physics;

SensorCollider::SensorCollider(Vector2f& center, Vector2f& horizontalDimensions, Vector2f& verticalDimensions) {
    this->center = center;
    _ceiling = construct(center - Vector2f(0, verticalDimensions.y / 2), verticalDimensions);
    _ground = construct(center + Vector2f(0, verticalDimensions.y / 2), verticalDimensions);
    _right = construct(center + Vector2f(horizontalDimensions.x / 2, 0), horizontalDimensions);
    _left = construct(center - Vector2f(horizontalDimensions.x / 2, 0), horizontalDimensions);
}

Collision SensorCollider::collides(const PillarCollider& collider) const {
    vector<FloatRect> groundCollisions = collider.intersects(_ground);
    vector<FloatRect> ceilingCollisions = collider.intersects(_ceiling);
    vector<FloatRect> leftCollisions = collider.intersects(_left);
    vector<FloatRect> rightCollisions = collider.intersects(_right);

    return Collision(groundCollisions, ceilingCollisions, leftCollisions, rightCollisions);
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
    return collides(platform.pillars);
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
    CustomUtilities::draw(ground()*ppu, Color::Green, target, states);
    CustomUtilities::draw(right()*ppu, Color::Red, target, states);
    CustomUtilities::draw(left()*ppu, Color::Blue, target, states);
    CustomUtilities::draw(ceiling()*ppu, Color::Cyan, target, states);
}