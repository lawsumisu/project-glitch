#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <utility>
#include "RectUtility.h"
#include "Collision.h"
#include "Platform.h"
#include "Line.h"

using namespace sf;
using namespace CustomUtilities;

namespace Physics{

    class SensorCollider : public Drawable {
    private:

        // Data Structure //
        // ============== //

        struct Constraint {
            sf::FloatRect bounds;
            std::pair<Line, Line> lines;

            Constraint(sf::FloatRect& bounds, std::pair<Line, Line>& lines);
        };
        enum class SensorType{GROUND, CEILING, LEFT, RIGHT};
        FloatRect _ceiling, _ground, _left, _right;
        FloatRect secondaryCeiling, secondaryGround, secondaryLeft, secondaryRight;
        Vector2f center;
        Vector2f size;
        sf::Vector2f gROrigin, cROrigin, lROrigin, rROrigin;

        std::vector<std::pair<size_t, sf::Vector2f>> findNearestWithinBounds(Constraint& constraints, SurfaceType type, 
            std::vector<pair<PlatformPtr, std::vector<Segment>>>& segmentList) const;

        bool within(sf::Vector2f& position, PlatformPtr& ptr, std::vector<sf::Vector2f>& boundPoints) const;

        void draw(sf::RenderTarget& target, sf::RenderStates states) const;

        

    public:
        SensorCollider() : SensorCollider(Vector2f(), Vector2f(), Vector2f()){}
        SensorCollider(Vector2f& center, Vector2f& horizontalDimensions, Vector2f& verticalDimensions);
        SensorCollider(Vector2f& center, Vector2f& size, Vector2f& horizontalRange, Vector2f& verticalRange);
        Collision collides(const PillarCollider& collider) const ;

        Collision collides(const vector<PillarCollider>& colliders) const ;

        std::vector<std::pair<size_t, float>> collides(const std::vector<PlatformPtr>& platforms, SurfaceType type) const;
        std::vector<std::pair<size_t, float>> checkSecondaryCollisions(const std::vector<PlatformPtr>& platforms,
            SurfaceType type) const;
        float groundAngle(const PlatformPtr& platform) const;
        std::pair<sf::Vector2f, sf::Vector2f> checkNarrow(const std::vector<PlatformPtr>& platforms, bool checkHorizontalNotVertical)const;

        FloatRect ceiling() const{ return FloatRect(_ceiling); }
        FloatRect ground() const{ return FloatRect(_ground); }
        FloatRect left() const{ return FloatRect(_left); }
        FloatRect right() const{ return FloatRect(_right); }
        void setCenter(Vector2f newCenter);

        Polygon getCollider(sf::Vector2f cPosition, sf::Vector2f fPosition) const;
        std::vector<std::pair<PlatformPtr, std::vector<Segment>>> collides(sf::Vector2f& cPosition, sf::Vector2f& fPosition, const std::vector<PlatformPtr>& platforms) const;

        std::vector<std::pair<size_t, sf::Vector2f>> findNearestCollision(sf::Vector2f& cPosition, sf::Vector2f& fPosition, SurfaceType type, 
            std::vector<std::pair<PlatformPtr, std::vector<Segment>>>& segmentList, float xMin, float xMax, float yMin, float yMax) const;
        std::vector<std::pair<size_t, sf::Vector2f>> findNearestSurface(sf::Vector2f& fPosition, SurfaceType type, 
            std::vector<std::pair<PlatformPtr, std::vector<Segment>>>& segmentList) const;
        std::vector<PlatformPtr> within(sf::Vector2f& position, std::vector<PlatformPtr>& platforms) const;
        bool within(sf::Vector2f& position, PlatformPtr& ptr) const;
    };
}

