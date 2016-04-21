#include "stdafx.h"
#include "Platform.h"
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include "GameState.h"
#include "MathUtility.h"

using namespace sf;
using namespace std;

Platform::Platform(PillarCollider pillars, Vector2f& velocity, float flipTime, PlatformType type) :
    _pillars(pillars), path(pillars.origin(), pillars.origin() + flipTime*velocity) {
    _type = type;
    this->_velocity = velocity;
}

Platform::Platform(PillarCollider pillars, PlatformType type) : Platform(pillars, Vector2f(0, 0), 0, type){}

void Platform::update() {

    Vector2f newOrigin = _pillars.origin();
    if ((direction == 1 && path.getT(newOrigin) >= 1) || (direction == -1 && path.getT(newOrigin) <= 0)) {
        _velocity *= -1.f;
        direction *= -1;
    }

    newOrigin += _velocity * GameState::time().gdt();
    _pillars.origin(newOrigin);
}

void Platform::origin(const Vector2f& newOrigin) {
    _pillars.origin(newOrigin);
    path = path.moveP1(newOrigin);
}

Vector2f Platform::velocity() const {
    if ((direction == 1 && path.getT(_pillars.origin()) >= 1) || (direction == -1 && path.getT(_pillars.origin()) <= 0)){
        return _velocity * -1.f;
    }
    else return _velocity;
}

void Platform::draw(RenderTarget& target, RenderStates states) const {
    target.draw(_pillars.toOutline(), states);
}