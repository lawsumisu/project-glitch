#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <utility>
#include "RectUtility.h"
#include "Collision.h"
#include "PillarCollider.h"
#include "Platform.h"

using namespace sf;
using namespace CustomUtilities;

namespace Physics{

    class SensorCollider : public Drawable {
    private:
        FloatRect _ceiling, _ground, _left, _right;
        Vector2f center;
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    public:
        SensorCollider() : SensorCollider(Vector2f(), Vector2f(), Vector2f()){}
        SensorCollider(Vector2f& center, Vector2f& horizontalDimensions, Vector2f& verticalDimensions);
        Collision collides(const PillarCollider& collider) const ;
        Collision collides(const vector<PillarCollider>& colliders) const ;

        Collision collides(const Platform& platform) const;
        /// <summary>
        /// Gets collision data from this <see cref="SensorCollider"/> and a series of <see cref="Platforms"/>.
        /// Returns a pair constituting collision data and location of the platform this sensor has ground contact with
        /// within the input list. This index is -1 if no contact is being made between this sensor's ground and a
        /// platform.
        /// </summary>
        /// <param name="platforms"></param>
        /// <returns></returns>
        std::pair<Collision, int> collides(const vector<Platform>& platforms) const ;

        FloatRect ceiling() const{ return FloatRect(_ceiling); }
        FloatRect ground() const{ return FloatRect(_ground); }
        FloatRect left() const{ return FloatRect(_left); }
        FloatRect right() const{ return FloatRect(_right); }
        void setCenter(Vector2f newCenter);
    };
}

