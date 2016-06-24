#include "stdafx.h"
#include "SensorCollider.h"
#include "VectorUtility.h"
#include "RectUtility.h"
#include "MathUtility.h"
#include "GameInfo.h"
#include "Line.h"

using namespace Physics;

SensorCollider::SensorCollider(Vector2f& center, Vector2f& horizontalDimensions, Vector2f& verticalDimensions) {
    this->center = center;
    _ceiling = construct(center - Vector2f(0, verticalDimensions.y / 2), verticalDimensions);
    _ground = construct(center + Vector2f(0, verticalDimensions.y / 2), verticalDimensions);
    _right = construct(center + Vector2f(horizontalDimensions.x / 2, 0), horizontalDimensions);
    _left = construct(center - Vector2f(horizontalDimensions.x / 2, 0), horizontalDimensions);
}

SensorCollider::SensorCollider(Vector2f& center, Vector2f& size, Vector2f& horizontalDimensions, Vector2f& verticalDimensions) {
    this->center = center;
    this->size = size;
    _ceiling = construct(center - Vector2f(0, verticalDimensions.y / 2), verticalDimensions);
    _ground = construct(center + Vector2f(0, verticalDimensions.y / 2), verticalDimensions);
    _right = construct(center + Vector2f(horizontalDimensions.x / 2, 0), horizontalDimensions);
    _left = construct(center - Vector2f(horizontalDimensions.x / 2, 0), horizontalDimensions);

    cROrigin = Vector2f(_ceiling.left - center.x, _ceiling.top - center.y);
    gROrigin = Vector2f(_ground.left - center.x, _ground.top - center.y);
    lROrigin = Vector2f(_left.left - center.x, _left.top - center.y);
    rROrigin = Vector2f(_right.left - center.x, _right.top - center.y);

    secondaryCeiling = FloatRect(_left.left, center.y - size.y / 2, _ceiling.width, size.y / 2);
    secondaryGround = FloatRect(_left.left, _ground.top, _ceiling.width, size.y / 2);
    secondaryLeft = FloatRect(_left.left, center.y - size.y / 2, _left.width, size.y);
    secondaryRight = FloatRect(_right.left, center.y - size.y / 2, _right.width, size.y);
}
Collision SensorCollider::collides(const PillarCollider& collider) const {
    vector<FloatRect> groundCollisions = collider.intersectsPillars(_ground);
    vector<FloatRect> ceilingCollisions = collider.intersectsPillars(_ceiling);
    vector<FloatRect> leftCollisions = collider.intersectsPillars(_left);
    vector<FloatRect> rightCollisions = collider.intersectsPillars(_right);

    vector<FloatRect> reducedGroundCollisions = {};
    for (FloatRect& r : groundCollisions) {
        if (_ground.top < r.top) reducedGroundCollisions.push_back(r);
    }

    return Collision(reducedGroundCollisions, ceilingCollisions, leftCollisions, rightCollisions);
}

Collision SensorCollider::collides(const vector<PillarCollider>& colliders) const {
    if (colliders.size() == 1) return collides(colliders[0]);

    Collision C = collides(colliders[0]);
    for (size_t i = 1U; i < colliders.size(); ++i) {
        C = C.reduce(collides(colliders[i]));
    }
    return C;

}


vector<pair<size_t, float>> SensorCollider::collides(const std::vector<PlatformPtr>& platforms, SurfaceType type) const {
    vector<pair<size_t, float>> output = {};
    size_t minIndex = 0;
    float nearestValue = 0;
    FloatRect collisionRect = _ground;
    if (type == SurfaceType::CEILING) collisionRect = _ceiling;
    else if (type == SurfaceType::RIGHT) collisionRect = _right;
    else if (type == SurfaceType::LEFT) collisionRect = _left;

    for (size_t i = 0; i < platforms.size(); ++i) {
        //Only collide with solid platforms for ceiling, left, and right collisions.
        if (type != SurfaceType::GROUND && platforms[i]->type() != PlatformType::SOLID) {
            continue;
        }

        pair<bool, float> collision = platforms[i]->collides(collisionRect, type);
        if (collision.first) {
            if (output.size() == 0 ||
                ((type == SurfaceType::GROUND|| type == SurfaceType::RIGHT) && collision.second < nearestValue) ||
                (type == SurfaceType::CEILING || type == SurfaceType::LEFT && collision.second > nearestValue)) {
                nearestValue = collision.second;
                minIndex = i;
            }
            output.push_back({ i, collision.second });
        }
    }

    // Add nearest value to end of list if not empty.
    if (output.size() != 0) output.push_back({ minIndex, nearestValue });
    return output;
}

float SensorCollider::groundAngle(const PlatformPtr& platform) const {
    return platform->groundAngle(_ground);
}

vector<pair<size_t, float>> SensorCollider::checkSecondaryCollisions(const std::vector<PlatformPtr>& platforms,
    SurfaceType type) const {
    vector<pair<size_t, float>> output = {};
    size_t minIndex = 0;
    float nearestValue = 0;
    FloatRect collisionRect = secondaryGround;
    if (type == SurfaceType::CEILING) collisionRect = secondaryCeiling;
    else if (type == SurfaceType::RIGHT) collisionRect = secondaryRight;
    else if (type == SurfaceType::LEFT) collisionRect = secondaryLeft;

    for (size_t i = 0; i < platforms.size(); ++i) {
        //Only collide with solid platforms for ceiling, left, and right collisions.
        if (type != SurfaceType::GROUND && platforms[i]->type() != PlatformType::SOLID) {
            continue;
        }

        pair<bool, float> collision = platforms[i]->collides(collisionRect, type, false);
        if (collision.first) {
            if (output.size() == 0 ||
                ((type == SurfaceType::GROUND || type == SurfaceType::RIGHT) && collision.second < nearestValue) ||
                (type == SurfaceType::CEILING || type == SurfaceType::LEFT && collision.second > nearestValue)) {
                nearestValue = collision.second;
                minIndex = i;
            }
            output.push_back({ i, collision.second });
        }
    }

    // Add nearest value to end of list if not empty.
    if (output.size() != 0) output.push_back({ minIndex, nearestValue });
    return output;
}
pair<Vector2f, Vector2f> SensorCollider::checkNarrow(const vector<PlatformPtr>& platforms, bool checkHorizontalNotVertical)  const{
    if (platforms.size() < 2) return{};
    FloatRect rect;
    if (checkHorizontalNotVertical) {
        rect = construct(center, Vector2f(size.x, _left.height));
    }
    else {
        rect = construct(center, Vector2f(_ground.width, size.y));
    }

    Vector2f min, max;
    int iMin, iMax;
    for (auto& ptr : platforms) {
        //Only look for collisions with solid platforms.
        if (ptr->type() != PlatformType::SOLID) continue;
        vector<Vector2f> points = ptr->collides(rect);        
        if (points.size() == 0) return{min, max};
        //Else, get the min and max points from the collision lists.        
        else if (checkHorizontalNotVertical) {
            //min and max points are measured along the vertical.
            for (size_t i = 0; i < points.size(); ++i) {
                if (points[i].y == rect.top || points[i].y == rect.top + rect.height) continue;
                if ((i != iMax && points[i].y < min.y) || iMin == -1) {
                    min = points[i];
                    iMin = i;
                }
                else if ((i != iMin && points[i].y > max.y) || iMax == -1) {
                    max = points[i];
                    iMax = i;
                }
            }
        }
        else {
            //min and max points are measured along the horizontal.
            for (size_t i = 0; i < points.size(); ++i) {
                if (points[i].x == rect.left || points[i].x == rect.left + rect.width) continue;
                if ((i != iMax && points[i].x < min.x) || iMin == -1) {
                    min = points[i];
                    iMin = i;
                }
                else if ((i != iMin && points[i].x > max.x) || iMax == -1) {
                    max = points[i];
                    iMax = i;
                }
            }
        }
    }
    if (iMax != iMin && iMax >= 0 && iMin >= 0) {
        return{ min,max };
    }
    else {
        return{ Vector2f(), Vector2f() };
    }
    
}
void SensorCollider::setCenter(Vector2f newCenter) {
    Vector2f delta = newCenter - center;
    if (delta.x != 0 || delta.y != 0) {
        center = newCenter;
        _ground.left += delta.x;
        _ground.top += delta.y;
        _ceiling.left += delta.x;
        _ceiling.top += delta.y;
        _left.left += delta.x;
        _left.top += delta.y;
        _right.left += delta.x;
        _right.top += delta.y;

        secondaryCeiling += delta;
        secondaryGround += delta;
        secondaryLeft += delta;
        secondaryRight += delta;
    }
}

Polygon SensorCollider::getCollider(Vector2f cPosition, Vector2f fPosition) const {
    Vector2f diff = fPosition - cPosition;
    Vector2f dimensions = { _right.width + _left.width, _ground.top + _ground.height - _ceiling.top };
    FloatRect cRect = construct(cPosition, dimensions);
    FloatRect fRect = construct(fPosition, dimensions);
    vector<Vector2f> cPoints = toPoints(cRect);
    vector<Vector2f> fPoints = toPoints(fRect);
    vector<Vector2f> points = {};
    float minX = min(cRect.left, fRect.left), maxX = max(cRect.left + dimensions.x, fRect.left + dimensions.x);
    float minY = min(cRect.top, fRect.top), maxY = max(cRect.top + dimensions.y, fRect.top + dimensions.y);
    float precision = .00001f;
    if (abs(diff.y) < precision || abs(diff.x) < precision) {
        //If diff is only along 1 dimensions, then just create rect of the bounds.
        return Polygon(FloatRect(minX, minY, maxX - minX, maxY - minY));
    }
    else {
        //There are 3 connected points that come from each rectangle. 
        //So, iterate through these points until we come to the excluded points, and then iterate through the points of the other rectangle.

        //Excluded points are points that do not have a point along the max bounds between these two rects.
        
        size_t i = 0;
        while (points.size() < 6){
            for (i; i <= cPoints.size() && points.size() < 6; ++i) {
                size_t j = i%cPoints.size();
                if (cPoints[j].x == minX || cPoints[j].x == maxX || cPoints[j].y == minY || cPoints[j].y == maxY) {
                    points.push_back(cPoints[j]);
                }
                else {
                    if (i > 0) points.push_back(fPoints[i - 1]);
                    break;
                }
            }
            for (i; i <= fPoints.size() && points.size() < 6; ++i) {
                size_t j = i%fPoints.size();
                if (fPoints[j].x == minX || fPoints[j].x == maxX || fPoints[j].y == minY || fPoints[j].y == maxY) {
                    points.push_back(fPoints[j]);
                }
                else {
                    if (i > 0) points.push_back(cPoints[i - 1]);
                    break;
                }
            }
        }
        return Polygon(points);
    }
}
vector<vector<Segment>> SensorCollider::collides(Vector2f& cPosition, Vector2f& fPosition, const vector<PlatformPtr>& platforms) const {
    Polygon colliderShape = getCollider(cPosition, fPosition);
    vector<vector<Segment>> output;
    size_t count = 0;
    for (size_t i = 0; i < platforms.size(); ++i) {
        PlatformPtr ptr = platforms[i];
        vector<Segment> segments = ptr->collides(colliderShape);
        output.push_back(segments);
    }
    return output;
}
pair<int, Vector2f> SensorCollider::findNearestWithinBounds(FloatRect& bounds, SurfaceType type, vector<vector<Segment>>& segmentList) const {
    Vector2f nearestCollision;
    int ncIndex = -1;
    for (size_t i = 0; i < segmentList.size(); ++i) {
        for (size_t j = 0; j < segmentList[i].size(); ++j) {
            pair<bool, Segment> innerLine = segmentList[i][j].findInnerLine(bounds);
            if (innerLine.first) {
                vector<Vector2f> points = { innerLine.second.start(), innerLine.second.end() };
                for (auto& p : points) {
                    if (ncIndex == -1 ||
                        (type == SurfaceType::GROUND && p.y < nearestCollision.y) ||
                        (type == SurfaceType::CEILING && p.y > nearestCollision.y) ||
                        (type == SurfaceType::RIGHT && p.x < nearestCollision.x) ||
                        (type == SurfaceType::LEFT && p.x > nearestCollision.x)) {
                        nearestCollision = p;
                        ncIndex = i;
                    }
                }
            }

        }
    }
    return{ ncIndex, nearestCollision };
}
pair<int, Vector2f> SensorCollider::findNearestCollision(Vector2f& cPosition, Vector2f& fPosition, SurfaceType type, 
    vector<vector<Segment>>& segmentList, float xMin, float xMax, float yMin, float yMax) const {

    //cout << xMin << " " << xMax << " " << yMin << " " << yMax << endl;
    Vector2f diff = fPosition - cPosition;

    //First need to get constraints for acceptable collisions.
    Vector2f relativeOrigin;
    Vector2f sensorDimensions;
    
    if (type == SurfaceType::GROUND) {
        relativeOrigin = gROrigin;
        sensorDimensions = Vector2f(_ground.width, _ground.height);
    }
    else if (type == SurfaceType::CEILING) {
        relativeOrigin = cROrigin;
        sensorDimensions = Vector2f(_ceiling.width, _ceiling.height);
    }
    else if (type == SurfaceType::LEFT) {
        relativeOrigin = lROrigin;
        sensorDimensions = Vector2f(_left.width, _left.height);
    }
    else if (type == SurfaceType::RIGHT) {
        relativeOrigin = rROrigin;
        sensorDimensions = Vector2f(_right.width, _right.height);
    }
    Line lineA(cPosition, fPosition), lineB(cPosition, fPosition);
    float precision = .00001f;
    if (abs(diff.x) < precision && abs(diff.y) < precision) {
        lineA = Line(1, 0, xMin);
        lineB = Line(1, 0, xMax);
    }
    Vector2f absoluteCOrigin = relativeOrigin + cPosition;
    Vector2f absoluteFOrigin = relativeOrigin + fPosition;
    if (sign(diff.x) == sign(diff.y)) {
        lineA = lineA.getParallelLineTo(relativeOrigin + cPosition + Vector2f(0, sensorDimensions.y));
        lineB = lineA.getParallelLineTo(relativeOrigin + cPosition + Vector2f(sensorDimensions.x, 0));
    }
    else {
        lineA = lineA.getParallelLineTo(relativeOrigin + cPosition);
        lineB = lineA.getParallelLineTo(relativeOrigin + cPosition + sensorDimensions);
    }


    //Tighten bounds to be based on specific collider.
    xMin = max(xMin, min(absoluteCOrigin.x, absoluteFOrigin.x) );
    xMax = min(xMax, max(absoluteCOrigin.x + sensorDimensions.x, absoluteFOrigin.x + sensorDimensions.x));
    yMin = max(yMin, min(absoluteCOrigin.y, absoluteFOrigin.y));
    yMax = min(yMax, max(absoluteCOrigin.y, absoluteFOrigin.y) + sensorDimensions.y);

    if (type == SurfaceType::GROUND) {
        xMin = absoluteFOrigin.x;
        xMax = xMin + sensorDimensions.x;
        //cout << xMin << " " << xMax << " " << yMin << " " << yMax << endl;
        //cout << _ground.left << " " << xMax << " " << _ground.top << " " << yMax << endl;
    }
    //Now find the nearest collision.
    FloatRect searchBounds(xMin, yMin, xMax - xMin, yMax - yMin);
    return findNearestWithinBounds(searchBounds, type, segmentList);
}

pair<int, Vector2f> SensorCollider::findNearestSurface(Vector2f& fPosition, SurfaceType type, vector<vector<Segment>>& segmentList) const {
    Vector2f relativeOrigin;
    Vector2f sensorDimensions;

    if (type == SurfaceType::GROUND) {
        relativeOrigin = gROrigin;
        sensorDimensions = Vector2f(_ground.width, _ground.height);
    }
    else if (type == SurfaceType::CEILING) {
        relativeOrigin = cROrigin;
        sensorDimensions = Vector2f(_ceiling.width, _ceiling.height);
    }
    else if (type == SurfaceType::LEFT) {
        relativeOrigin = lROrigin;
        sensorDimensions = Vector2f(_left.width, _left.height);
    }
    else if (type == SurfaceType::RIGHT) {
        relativeOrigin = rROrigin;
        sensorDimensions = Vector2f(_right.width, _right.height);
    }

    FloatRect searchBounds(relativeOrigin.x + fPosition.x, relativeOrigin.y + fPosition.y, sensorDimensions.x, sensorDimensions.y);
    return findNearestWithinBounds(searchBounds, type, segmentList);
}

void SensorCollider::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    CustomUtilities::draw(ground(), Color::Green, target, states);
    CustomUtilities::draw(right(), Color::Red, target, states);
    CustomUtilities::draw(left(), Color::Blue, target, states);
    CustomUtilities::draw(ceiling(), Color::Cyan, target, states);

    int opacity = 170;
    /*CustomUtilities::draw(secondaryGround, Color(0,255,0,opacity), target, states);
    CustomUtilities::draw(secondaryRight, Color(255,0,0,opacity), target, states);
    
   CustomUtilities::draw(secondaryCeiling, Color(0,255,255,opacity), target, states);
    CustomUtilities::draw(secondaryLeft, Color(0, 0, 255, opacity), target, states);*/

}