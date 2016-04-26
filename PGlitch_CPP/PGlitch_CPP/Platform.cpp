#include "stdafx.h"
#include "Platform.h"
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include "GameState.h"
#include "MathUtility.h"

using namespace sf;
using namespace std;

Platform::Platform(PillarCollider pillars, Vector2f& velocity, float flipTime, PlatformType type) :
    _pillars(pillars), path(_position, _position + flipTime*velocity), _velocity(velocity) {
    _type = type;
}

Platform::Platform(PillarCollider pillars, PlatformType type) : Platform(pillars, Vector2f(0, 0), 0, type){}

void Platform::update() {

    Vector2f p = _position;
    if ((direction == 1 && path.getT(p) >= 1) || (direction == -1 && path.getT(p) <= 0)) {
        _velocity *= -1.f;
        direction *= -1;
    }

    p += _velocity * GameState::time().gdt();
    position(p);
}

pair<bool, float> Platform::collides(const sf::FloatRect& rect, SurfaceType type) const {
    return _pillars.intersects(T, rect, type);
}
float Platform::groundAngle(const sf::FloatRect& rect) const {
    Vector2f LR = linearRegression(_pillars.findSurfacePoints(T, rect));
    if (isnan(LR.y)) return 0;
    else return atan(-LR.y);
}

void Platform::position(const Vector2f& newPosition) {
    //_pillars.origin(newOrigin);
    //path = path.moveP1(newOrigin);
    _position = newPosition;
    T = Transform().translate(_position);
}

void Platform::draw(RenderTarget& target, RenderStates states) const {
    _pillars.draw(T, target, states, Color::White);
}