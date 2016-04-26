#pragma once

#include "PillarCollider.h"
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Color.hpp>
#include "Enumerators.h"
#include "Platform2.h"

using namespace Physics;

namespace Physics {
    class SensorCollider; //Forward declaration for friendship.
}

class Platform : public Platform2 {
private:
    //Fields

    PillarCollider _pillars;
    Vector2f _velocity;
    Vector2f _position;
    float lastFlippedTime;
    sf::Transform T;

    int direction = 1;
    Line path;

    // Type this platform is. Solid platforms have hard surfaces that can be collided with.
    // Thin and thick platforms can only be collided with from above. Thin plaforms can also be passed
    // through while falling.
    PlatformType _type = PlatformType::THICK;

    //Methods

    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
public:

    //Constructor
    Platform(PillarCollider pillars, sf::Vector2f& velocity, float flipTime = 1.f, PlatformType type = PlatformType::SOLID);
    Platform(PillarCollider pillars, PlatformType type = PlatformType::SOLID);

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

    const PillarCollider& pillars() const {
        return _pillars;
    }

    friend class SensorCollider;

    // ================= //
    // Inherited Methods //
    // ================= //

    const sf::Transform& transform() const { return T; }
    std::pair<bool, float> collides(const sf::FloatRect& rect, SurfaceType type) const;
    float groundAngle(const sf::FloatRect& rect) const;
};

