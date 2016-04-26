#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <utility>
#include "RectUtility.h"
#include "Collision.h"
#include "PillarCollider.h"
#include "Platform.h"
#include "Platform2.h"

using namespace sf;
using namespace CustomUtilities;

namespace Physics{

    class SensorCollider : public Drawable {
    private:
        enum class SensorType{GROUND, CEILING, LEFT, RIGHT};
        FloatRect _ceiling, _ground, _left, _right;
        Vector2f center;

        void draw(sf::RenderTarget& target, sf::RenderStates states) const;

        std::vector<std::pair<size_t, float>> collides(const std::vector<Platform>& platforms, SensorType type) const;
    public:
        SensorCollider() : SensorCollider(Vector2f(), Vector2f(), Vector2f()){}
        SensorCollider(Vector2f& center, Vector2f& horizontalDimensions, Vector2f& verticalDimensions);
        SensorCollider(Vector2f& center, Vector2f& size, Vector2f& horizontalRange, Vector2f& verticalRange);
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

        /// <summary>
        /// Gets collision data from this <see cref="SensorCollider"/> and a <see cref="PillarCollider"/>.
        /// Specificially returns a <see cref="Collision"/> for left and right collisions.
        /// </summary>
        Collision collidesHorizontal(const PillarCollider& collider) const;

        /// <summary>
        /// Gets collision data from this <see cref="SensorCollider"/> and a <see cref="PillarCollider"/>.
        /// Specificially returns a <see cref="Collision"/> for ground and ceiling collisions.
        /// </summary>
        Collision collidesVertical(const PillarCollider& collider) const;

        /// <summary>
        /// Gets collision data from this <see cref="SensorCollider"/> and a series of <see cref="PillarColliders"/>.
        /// Return a pair constituting collision data and the indices of the pillars this sensor has collided with
        /// within the input list.
        /// Given an output o, o.second.first is the index for a left collision, and o.second.second is the index for a 
        /// right collision. If there is no collision on the right or left, the value is -1 accordingly.
        /// </summary>
        /// <param name="colliders"></param>
        /// <returns></returns>
        std::pair<Collision, std::pair<int, int>> collidesHorizontal(const vector<PillarCollider>& colliders) const;
        std::pair<Collision, std::pair<int, int>> collidesVertical(const vector<PillarCollider>& colliders) const;


        std::vector<std::pair<size_t, float>> collidesGround(const std::vector<Platform>& platforms) const;
        std::vector<std::pair<size_t, float>> collidesCeiling(const std::vector<Platform>& platforms) const;
        std::vector<std::pair<size_t, float>> collidesLeft(const std::vector<Platform>& platforms) const;
        std::vector<std::pair<size_t, float>> collidesRight(const std::vector<Platform>& platforms) const;
        float groundAngle(const Platform& platform) const;

        std::vector<std::pair<size_t, float>> collides(const std::vector<PlatformPtr>& platforms, SurfaceType type) const;
        float groundAngle(const PlatformPtr& platform) const;

        FloatRect ceiling() const{ return FloatRect(_ceiling); }
        FloatRect ground() const{ return FloatRect(_ground); }
        FloatRect left() const{ return FloatRect(_left); }
        FloatRect right() const{ return FloatRect(_right); }
        void setCenter(Vector2f newCenter);
    };
}

