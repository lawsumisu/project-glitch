#include "stdafx.h"
#include "Platform2.h"
#include "MathUtility.h"
#include "GameState.h"

using namespace sf;
using namespace std;
using namespace CustomUtilities;

AffinePlatform::AffinePlatform(PolygonalCollider collider, PlatformType type, float angularVelocity) :
    collider(collider){
    this->angularVelocity = angularVelocity;
    _type = type;
}

void AffinePlatform::update() {
    _angle += angularVelocity*GameState::time().gdt();
    createTransform();
}

pair<bool, float> AffinePlatform::collides(const FloatRect& r, SurfaceType type) const {
    if (_type != PlatformType::SOLID && type != SurfaceType::GROUND) {
        //Non solid platforms can only have ground collisions.
        //return{ false, 0.f };
    }
    return collider.intersects(T, r, type);
}

float AffinePlatform::groundAngle(const FloatRect& rect) const {
    Vector2f LR = linearRegression(collider.findSurfacePoints(T, rect));
    if (isnan(LR.y)) return 0;
    else return atan(-LR.y);
}

void AffinePlatform::createTransform() {
    T = Transform().translate(_position).rotate(_angle).translate(_rotationalOffset);
}
void AffinePlatform::angle(float newAngle) {
    if (_angle != newAngle) {
        _angle = newAngle;
        createTransform();
    }
}

void AffinePlatform::position(const Vector2f& newPosition) {
    if (_position != newPosition) {
        _position = newPosition;
        createTransform();
    }
}

void AffinePlatform::rotationalOffset(Vector2f newRotationalOffset) {
    if (_rotationalOffset != newRotationalOffset) {
        _rotationalOffset = newRotationalOffset;
        createTransform();
    }
}

void AffinePlatform::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    collider.draw(T, target, states, Color::White);
}