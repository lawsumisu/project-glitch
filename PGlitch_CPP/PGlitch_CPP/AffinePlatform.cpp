#include "stdafx.h"
#include "Platform.h"
#include "MathUtility.h"
#include "GameState.h"

using namespace sf;
using namespace std;
using namespace CustomUtilities;

AffinePlatform::AffinePlatform(FreeformCollider collider, PlatformType type, float angularVelocity) :
    collider(collider){
    this->angularVelocity = angularVelocity;
    _type = type;
}

void AffinePlatform::update() {
    _angle += angularVelocity*GameState::time().gdt();
    createTransform();
}

float AffinePlatform::groundAngle(const FloatRect& rect) const {
    vector<Segment> foundLines = collider.intersects(Polygon(rect), T);

    //Only want the top most lines, so need to sort these segments and find those segments.
    //Sort found lines by lines with the highest (minimum) start or end point.
    sort(foundLines.begin(), foundLines.end(), [](const Segment& l1, const Segment& l2) {
        float y1 = min(l1.start().y, l1.end().y);
        float y2 = min(l2.start().y, l2.start().y);
        return y1 > y2;
    });

    vector<FloatRect> rects = {};
    vector<Vector2f> topPoints = {};
    for (const Segment& l : foundLines) {
        FloatRect lRect(min(l.start().x, l.end().x), min(l.start().y, l.end().x),
            abs(l.start().x - l.end().x), abs(l.start().y - l.end().y));
        bool isLegitimate = true;
        for (const FloatRect& r : rects) {
            if (r.intersects(lRect)) {
                isLegitimate = false;
                break;
            }
        }
        if (isLegitimate) {
            topPoints.push_back(l.start());
            topPoints.push_back(l.end());
            //cout << l.toString() << endl;
        }
        rects.push_back(lRect);
    }
    //sort output according to smallest x coordinate to largest x coordinate.

    sort(topPoints.begin(), topPoints.end(), [](const Vector2f& v1, const Vector2f& v2) {
        return v1.x < v2.x;
    });

    Vector2f LR = linearRegression(topPoints);
    if (isnan(LR.y)) return 0;
    else return atan(-LR.y);
}

void AffinePlatform::createTransform() {
    T = Transform().translate(_position).rotate(_angle, _rotationalOffset);
}

vector<Segment> AffinePlatform::collides(Polygon& shape) const {
    return collider.intersects(shape, T);
}

bool AffinePlatform::contains(Vector2f& v) const {
    return collider.contains(v, T);
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

void AffinePlatform::draw(sf::RenderTarget& target, sf::RenderStates states, bool debug) const {
    collider.draw(T, target, states, Color::White, debug);
}

void AffinePlatform::draw(const Color& color, RenderTarget& target, RenderStates states) const {
    collider.draw(T, target, states, color, false);
}