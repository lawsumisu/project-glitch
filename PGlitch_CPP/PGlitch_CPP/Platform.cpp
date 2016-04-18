#include "stdafx.h"
#include "Platform.h"
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include "GameState.h"
#include "MathUtility.h"

using namespace sf;
using namespace std;

Platform::Platform(PillarCollider pillars, Vector2f& velocity, float flipTime) :
    pillars(pillars), path(pillars.origin(), pillars.origin() + flipTime*velocity) {
    this->_velocity = velocity;
}

void Platform::update() {

    Vector2f newOrigin = pillars.origin();
    if ((direction == 1 && path.getT(newOrigin) >= 1) || (direction == -1 && path.getT(newOrigin) <= 0)) {
        _velocity *= -1.f;
        direction *= -1;
    }

    newOrigin += _velocity * GameState::time().gdt();
    pillars.origin(newOrigin);
}

void Platform::origin(const Vector2f& newOrigin) {
    pillars.origin(newOrigin);
    path = path.moveP1(newOrigin);
}

Vector2f Platform::velocity() const {
    return _velocity;
}
void Platform::draw(RenderTarget& target, RenderStates states) const {
    target.draw(pillars.toOutline(), states);
}