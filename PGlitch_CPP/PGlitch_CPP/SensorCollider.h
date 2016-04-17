#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include "RectUtility.h"
#include "Collision.h"
#include "PillarCollider.h"

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
        Collision collides(PillarCollider& collider);
        Collision collides(vector<PillarCollider>& colliders);
        FloatRect ceiling() const{ return FloatRect(_ceiling); }
        FloatRect ground() const{ return FloatRect(_ground); }
        FloatRect left() const{ return FloatRect(_left); }
        FloatRect right() const{ return FloatRect(_right); }
        void setCenter(Vector2f newCenter);
    };
}

