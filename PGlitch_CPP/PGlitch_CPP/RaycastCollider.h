#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include "PillarCollider.h"

using namespace sf;

namespace Physics {
    class RaycastCollider : public Drawable {
    private:
        //Fields

        //Angle rays are produced in. Measured in radians. 
        //An increase in angle corresponds to moving clockwise around the unit circle.
        float _angle;

        //Center of this collider.
        sf::Vector2f origin;

        //Dimensions of this collider. Requires that size.x and size.y > 0.
        //size.x / 2 corresponds to the length of rays.
        //size.y corresponds to the width of the line that rays are produced perpendicular to.
        sf::Vector2f size;

        //Number of rays that are produced.
        //RI: >= 2.
        size_t rayCount = 2;

        std::vector<Line> rays;

        //Methods

        void constructRays();

        void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    public:
        //Constructor
        RaycastCollider(const sf::Vector2f& origin, const sf::Vector2f& size, float angle, size_t rayCount = 2);

        //Methods

        std::pair<bool, float> collides(PillarCollider& pillar) const;

        void setOrigin(const Vector2f& origin);


        // Accessors
        void angle(float newAngle);
        float angle() const;
    };
}