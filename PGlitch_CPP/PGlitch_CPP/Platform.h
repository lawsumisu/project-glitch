#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <utility>
#include "Enumerators.h"
#include "FreeformCollider.h"
#include "PillarCollider.h"
#include <memory>
#include <iostream>
#include "VectorUtility.h"
#include "Polygon.h"

//Interface for platforms.
class Platform : public sf::Drawable{
protected:
    // Type this platform is. Solid platforms have hard surfaces that can be collided with.
    // Thin and thick platforms can only be collided with from above. Thin plaforms can also be passed
    // through while falling.
    PlatformType _type;

public:
    virtual void update() = 0;
    virtual void position(const Vector2f& newPosition) = 0;
    virtual float groundAngle(const sf::FloatRect& rect) const = 0;
    void draw(sf::RenderTarget& target, sf::RenderStates states)const { draw(target, states, true); }
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states, bool debug) const = 0;
    virtual void draw(const sf::Color& color, sf::RenderTarget& target, sf::RenderStates states) const = 0;
    virtual const sf::Transform& transform() const = 0;
    virtual std::vector<CustomUtilities::Segment> collides(Polygon& shape) const = 0;

    /// <summary>
    /// Checks whether or a not a point exists within this platform.
    /// </summary>
    /// <param name="v"></param>
    /// <returns></returns>
    virtual bool contains(sf::Vector2f& v) const = 0;

    PlatformType type() const { return _type; }
};

class SimplePlatform : public Platform {
private:
    //Fields

    Physics::PillarCollider _pillars;
    Vector2f _velocity;
    Vector2f _position;
    float lastFlippedTime;
    sf::Transform T;

    int direction = 1;
    Segment path;


    //Methods
public:

    //Constructor
    SimplePlatform(Physics::PillarCollider pillars, sf::Vector2f& velocity, float flipTime = 1.f, 
        PlatformType type = PlatformType::SOLID);
    SimplePlatform(Physics::PillarCollider pillars, PlatformType type = PlatformType::SOLID);

    //Methods

    void update();

    void position(const sf::Vector2f& newPosition);

    /// <summary>
    /// Gets the type of this <see cref="Platform"/>.
    /// </summary>
    /// <returns></returns>
    PlatformType type() const {
        return _type;
    }

    // ================= //
    // Inherited Methods //
    // ================= //

    const sf::Transform& transform() const { return T; }
    bool contains(sf::Vector2f& v) const;

    std::vector<CustomUtilities::Segment> collides(Polygon& shape) const;
    float groundAngle(const sf::FloatRect& rect) const;
    void draw(sf::RenderTarget& target, sf::RenderStates states, bool debug) const;
    void draw(const sf::Color& color, sf::RenderTarget& target, sf::RenderStates states) const;
    
};

class AffinePlatform : public Platform {
private:
    //Fields
    FreeformCollider collider;
    float _angle;
    sf::Vector2f _position;
    sf::Vector2f _rotationalOffset;
    float angularVelocity;

    sf::Transform T;

    //Methods 
    void createTransform();

public:
    //Constructor
    AffinePlatform(FreeformCollider collider, PlatformType type = PlatformType::SOLID, float angularVelocity = 0.f);

    //Methods

    void angle(float newAngle) ;
    void position(const sf::Vector2f& newPosition) ;
    void rotationalOffset(sf::Vector2f newRotationalOffset) ;

    void update();
    const sf::Transform& transform() const {
        return T;
    }
    bool contains(sf::Vector2f& v) const;

    std::vector<CustomUtilities::Segment> collides(Polygon& shape) const;
    float groundAngle(const sf::FloatRect& rect) const;
    void draw(sf::RenderTarget& target, sf::RenderStates states, bool debug) const;
    void draw(const sf::Color& color, sf::RenderTarget& target, sf::RenderStates states) const;

};

typedef std::shared_ptr<Platform> PlatformPtr;
