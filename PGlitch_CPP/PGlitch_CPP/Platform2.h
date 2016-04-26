#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <utility>
#include "Enumerators.h"
#include "PolygonalCollider.h"
#include <memory>
#include <iostream>
#include "VectorUtility.h"

//Interface for platforms.
class Platform2 : public sf::Drawable{
protected:
    PlatformType _type;

public:
    virtual void update() = 0;
    virtual void position(const Vector2f& newPosition) = 0;
    virtual std::pair<bool, float> collides(const sf::FloatRect& rect, SurfaceType direction) const = 0;
    virtual float groundAngle(const sf::FloatRect& rect) const = 0;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
    virtual const sf::Transform& transform() const = 0;
    PlatformType type() const { return _type; }
};

class AffinePlatform : public Platform2 {
private:
    //Fields
    PolygonalCollider collider;
    float _angle;
    sf::Vector2f _position;
    sf::Vector2f _rotationalOffset;
    float angularVelocity;

    sf::Transform T;

    //Methods 
    void createTransform();
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

public:
    //Constructor
    AffinePlatform(PolygonalCollider collider, PlatformType type = PlatformType::SOLID, float angularVelocity = 0.f);

    //Methods

    void angle(float newAngle) ;
    void position(const sf::Vector2f& newPosition) ;
    void rotationalOffset(sf::Vector2f newRotationalOffset) ;

    void update();
    const sf::Transform& transform() const {
        return T;
    }
    std::pair<bool, float> collides(const sf::FloatRect& rect, SurfaceType type) const;
    float groundAngle(const sf::FloatRect& rect) const;
};

typedef std::shared_ptr<Platform2> PlatformPtr;
