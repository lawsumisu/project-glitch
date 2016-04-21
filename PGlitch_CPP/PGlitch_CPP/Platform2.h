#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <utility>
#include "Enumerators.h"
#include "PolygonalCollider.h"
//Interface for platforms
class Platform2 {
protected:
    PlatformType _type;

public:
    virtual std::pair<bool, float> collides(const sf::FloatRect& rect, CollisionDirection direction) const = 0;
    PlatformType type() { return _type; }
};

class TransformablePlatform : public Platform2 {
private:
    //Fields
    PolygonalCollider collider;
    float _angle;
    sf::Vector2f _position;
    sf::Vector2f _rotationalOffset;


public:
    //Constructor
    TransformablePlatform(PolygonalCollider collider);

    //Methods

    void angle(float newAngle) ;
    void position(sf::Vector2f newPosition) ;
    void rotationalOffset(sf::Vector2f newRotationalOffset) ;
    std::pair<bool, float> collides(const sf::FloatRect& rect, CollisionDirection direction) const;
};

using namespace sf;
using namespace std;

TransformablePlatform::TransformablePlatform(PolygonalCollider collider) : collider(collider){}

pair<bool, float> TransformablePlatform::collides(const FloatRect& r, CollisionDirection direction) const {
    return{ false, 0.f };
}

void TransformablePlatform::angle(float newAngle) { _angle = newAngle; }

void TransformablePlatform::position(Vector2f newPosition) { _position = newPosition; }

void TransformablePlatform::rotationalOffset(Vector2f newRotationalOffset) { _rotationalOffset = newRotationalOffset; }
