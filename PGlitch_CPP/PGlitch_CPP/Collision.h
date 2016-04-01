#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <vector>
#include "RectUtility.h"
#include <sstream>

using namespace sf;
using namespace std;
using namespace CustomUtilities;

namespace Physics {
    class Collision {
    private:
        enum class WallType { LEFT, RIGHT, CEILING, GROUND };
        float _groundAngle;
        FloatRect _ground, _ceiling, _left, _right;
        FloatRect findNearestCollision(vector<FloatRect>& collisions, WallType type);
        float calculateAngle(vector<FloatRect>& groundCollisions);

        //Constructors
        Collision(FloatRect& G, FloatRect& C, FloatRect& L, FloatRect& R, float angle);
    public:
        Collision(vector<FloatRect>& groundCollisions, vector<FloatRect>& ceilingCollisions,
            vector<FloatRect>& leftCollisions, vector<FloatRect>& rightCollisions);

        /// <summary>
        /// Compares this <see cref="Collision"/> to another and returns a single <see cref="Collision"/>
        /// that represents the nearest collisions between the two of them in the four directions: 
        /// ground, ceiling, left, right.
        /// </summary>
        Collision reduce(Collision& otherCollision) const;
        
        // Accessors

        float groundAngle() const { return _groundAngle; }
        float ground() const { return _ground.top; }
        float ceiling() const { return _ceiling.top + _ceiling.height; }
        float left() const { return _left.left + _left.width; }
        float right() const { return _right.left; }
        bool withGround() const { return _ground.height * _ground.width != 0; }
        bool withCeiling() const { return _ceiling.height * _ceiling.width != 0; }
        bool withLeft() const { return _left.height * _left.width != 0; }
        bool withRight() const { return _right.height * _right.width != 0; }
        /// <summary>
        /// Returns a string that composes all of the collision information of this
        /// <see cref="Collision"/>. 
        /// Format is 'Collision: ground(g), ceiling(c), left(l), right(r)', where 
        /// g, c, l, and r are the corresponding x or y coordinate of the collision.
        /// If there is no collision at a certain surface, then it is omited from the output.
        /// </summary>
        /// <returns></returns>
        string toString() const;
    };
}
