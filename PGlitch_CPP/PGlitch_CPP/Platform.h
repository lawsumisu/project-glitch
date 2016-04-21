#pragma once

#include "PillarCollider.h"
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Color.hpp>
#include "Enumerators.h"

using namespace Physics;

namespace Physics {
    class SensorCollider; //Forward declaration for friendship.
}

class Platform : public sf::Drawable {
private:
    //Fields

    PillarCollider _pillars;
    Vector2f _velocity;

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

    void origin(const sf::Vector2f& newOrigin);

    sf::Vector2f velocity() const;

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
};

