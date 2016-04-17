#include "stdafx.h"
#include "SensorCollider.h"

using namespace Physics;
SensorCollider::SensorCollider(Vector2f& center, Vector2f& horizontalDimensions, Vector2f& verticalDimensions) {
    this->center = center;
    _ceiling = construct(center - Vector2f(0, verticalDimensions.y / 2), verticalDimensions);
    _ground = construct(center + Vector2f(0, verticalDimensions.y / 2), verticalDimensions);
    _right = construct(center + Vector2f(horizontalDimensions.x / 2, 0), horizontalDimensions);
    _left = construct(center - Vector2f(horizontalDimensions.x / 2, 0), horizontalDimensions);
}

Collision SensorCollider::collides(PillarCollider& collider) {
    vector<FloatRect> groundCollisions = collider.intersects(_ground);
    vector<FloatRect> ceilingCollisions = collider.intersects(_ceiling);
    vector<FloatRect> leftCollisions = collider.intersects(_left);
    vector<FloatRect> rightCollisions = collider.intersects(_right);

    return Collision(groundCollisions, ceilingCollisions, leftCollisions, rightCollisions);
}

Collision SensorCollider::collides(vector<PillarCollider>& colliders) {
    if (colliders.size() == 1) return collides(colliders[0]);

    Collision C = collides(colliders[0]);
    for (size_t i = 1U; i < colliders.size(); ++i) {
        C = C.reduce(collides(colliders[i]));
    }
    return C;

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
    CustomUtilities::draw(ground(), Color(0,0,255,255), target, states);
    CustomUtilities::draw(right(), Color(255, 0, 0, 255), target, states);
    CustomUtilities::draw(left(), Color(0, 255, 0, 255), target, states);
    CustomUtilities::draw(ceiling(), Color(0, 255, 255, 255), target, states);
}