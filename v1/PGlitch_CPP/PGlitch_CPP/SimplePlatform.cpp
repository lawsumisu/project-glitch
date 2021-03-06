#include "stdafx.h"
#include "Platform.h"
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include "GameState.h"
#include "MathUtility.h"

using namespace sf;
using namespace std;
using namespace Physics;
using namespace CustomUtilities;

SimplePlatform::SimplePlatform(PillarCollider pillars, Vector2f& velocity, float flipTime, PlatformType type) :
    _pillars(pillars), path(Vector2f(), Vector2f() + flipTime*velocity), _velocity(velocity) {
    _type = type;
}

SimplePlatform::SimplePlatform(PillarCollider pillars, PlatformType type) : SimplePlatform(pillars, Vector2f(0, 0), 0, type){}

void SimplePlatform::update() {

    Vector2f p = _position;
    if ((direction == 1 && path.getT(p) >= 1) || (direction == -1 && path.getT(p) <= 0)) {
        _velocity *= -1.f;
        direction *= -1;
    }

    _position += _velocity * GameState::time().gdt();
    T = Transform().translate(_position);

    
}

vector<Segment> SimplePlatform::collides(Polygon& shape) const {
    vector<Segment> segments = _pillars.intersects(shape.transform(T.getInverse()));
    for (size_t i = 0; i < segments.size(); ++i) segments[i] = segments[i].transform(T);
    return segments;
}

bool SimplePlatform::contains(Vector2f& v) const {
    return _pillars.contains(v, T);
}

float SimplePlatform::groundAngle(const sf::FloatRect& rect) const {
    Vector2f LR = linearRegression(_pillars.findSurfacePoints(T, rect));
    if (isnan(LR.y)) return 0;
    else return atan(-LR.y);
}

void SimplePlatform::position(const Vector2f& newPosition) {
    //_pillars.origin(newOrigin);
    //path = path.moveP1(newOrigin);
    _position = newPosition;
    path = path.moveP1(newPosition);
    T = Transform().translate(_position);
}

void SimplePlatform::draw(RenderTarget& target, RenderStates states, bool debug) const {
    _pillars.draw(T, target, states, Color::White, debug);
}

void SimplePlatform::draw(const Color& color, RenderTarget& target, RenderStates states) const {
    _pillars.draw(T, target, states, color, false);
}