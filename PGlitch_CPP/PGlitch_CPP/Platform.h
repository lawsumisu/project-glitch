#pragma once

#include "PillarCollider.h"
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Color.hpp>

using namespace Physics;

namespace Physics {
    class SensorCollider; //Forward declaration for friendship.
}

class Platform : public sf::Drawable {
private:
    //Fields

    PillarCollider pillars;
    Vector2f _velocity;

    int direction = 1;
    Line path;

    //Methods

    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
public:

    //Constructor
    Platform(PillarCollider pillars, sf::Vector2f& velocity, float flipTime = 1.f);

    //Methods

    void update();

    void origin(const sf::Vector2f& newOrigin);

    sf::Vector2f velocity() const;

    friend class SensorCollider;
};

