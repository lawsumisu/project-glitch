#include "stdafx.h"
#include "SensorCollider.h"
#include "VectorUtility.h"
#include "RectUtility.h"
#include "MathUtility.h"
#include "GameInfo.h"

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

SensorCollider::SensorCollider(Vector2f& center, Vector2f& size, float armLength, float legLength): center(center), size(size) {
    Vector2f hDimensions(size.x / 2, armLength * 2);
    cout << toString(hDimensions) << endl;
    _right = construct(center + Vector2f(hDimensions.x / 2, 0), hDimensions);
    _left = construct(center - Vector2f(hDimensions.x / 2, 0), hDimensions);

    _ground = construct(center + Vector2f(0, size.y / 2), Vector2f(size.x - 1, legLength * 2));
    _ceiling = construct(center - Vector2f(0, size.y / 4), Vector2f(size.x - 1, size.y / 2));

    cROrigin = Vector2f(_ceiling.left - center.x, _ceiling.top - center.y);
    gROrigin = Vector2f(_ground.left - center.x, _ground.top - center.y);
    lROrigin = Vector2f(_left.left - center.x, _left.top - center.y);
    rROrigin = Vector2f(_right.left - center.x, _right.top - center.y);
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

float SensorCollider::groundAngle(const PlatformPtr& platform) const {
    return platform->groundAngle(_ground);
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
vector<pair<PlatformPtr, vector<Segment>>> SensorCollider::collides(Vector2f& cPosition, Vector2f& fPosition, const vector<PlatformPtr>& platforms) const {
    Polygon colliderShape = getCollider(cPosition, fPosition);
    vector<pair<PlatformPtr, vector<Segment>>> output;
    size_t count = 0;
    for (size_t i = 0; i < platforms.size(); ++i) {
        PlatformPtr ptr = platforms[i];
        vector<Segment> segments = ptr->collides(colliderShape);
        output.push_back({ ptr, segments });
    }
    return output;
}

bool SensorCollider::within(sf::Vector2f& position, PlatformPtr& ptr, vector<Vector2f>& boundPoints) const {
    for (auto& v : boundPoints) {
        if (ptr->contains(v)) return true;
    }
    return false;
}

bool SensorCollider::within(Vector2f& position, PlatformPtr& ptr) const {
    FloatRect bounds = construct(position, size * .99f);
    //Ignore ground.
    bounds.height = bounds.height / 2;
    vector<Vector2f> boundPoints = toPoints(bounds);
    return within(position, ptr, boundPoints);
}
vector<PlatformPtr> SensorCollider::within(sf::Vector2f& position, std::vector<PlatformPtr>& platforms) const {
    FloatRect bounds = construct(position, size * .99f);
    //Ignore ground.
    bounds.height = bounds.height / 2;
    vector<Vector2f> boundPoints = toPoints(bounds);
    vector<PlatformPtr> output;
    for (auto& ptr : platforms) {
        if (within(position, ptr, boundPoints)) output.push_back(ptr);
    }
    return output;
}
vector<pair<size_t, Vector2f>> SensorCollider::findNearestWithinBounds(Constraint& constraint, SurfaceType type, 
    vector<pair<PlatformPtr, vector<Segment>>>& segmentList) const {
    vector<pair<size_t, Vector2f>> output;
    Vector2f nearestCollision;
    int ncIndex = -1;
    for (size_t i = 0; i < segmentList.size(); ++i) {
        Vector2f ithNearestCollision;
        bool foundCollision = false;
        if (type != SurfaceType::GROUND && (segmentList[i].first->type() == PlatformType::THICK || segmentList[i].first->type() == PlatformType::THIN)) {
            continue;
        }
        for (size_t j = 0; j < segmentList[i].second.size(); ++j) {
            //if (type == SurfaceType::GROUND && segmentList[i][j].start().x == segmentList[i][j].end().x) continue;
            pair<bool, Segment> innerLine = segmentList[i].second[j].findInnerLine(constraint.bounds);
            if (innerLine.first) {
                vector<Vector2f> points = { innerLine.second.start(), innerLine.second.end() };
                for (auto& p : points) {
                    //if (constraint.lines.first.sign(p) == constraint.lines.second.sign(p)) continue;
                    if (ncIndex == -1 ||
                        (type == SurfaceType::GROUND && p.y < nearestCollision.y) ||
                        (type == SurfaceType::CEILING && p.y > nearestCollision.y) ||
                        (type == SurfaceType::RIGHT && p.x < nearestCollision.x) ||
                        (type == SurfaceType::LEFT && p.x > nearestCollision.x)) {
                        nearestCollision = p;
                        ncIndex = i;
                    }
                    if (!foundCollision ||
                        (type == SurfaceType::GROUND && p.y < ithNearestCollision.y) ||
                        (type == SurfaceType::CEILING && p.y > ithNearestCollision.y) ||
                        (type == SurfaceType::RIGHT && p.x < ithNearestCollision.x) ||
                        (type == SurfaceType::LEFT && p.x > ithNearestCollision.x)) {
                        ithNearestCollision = p;
                        foundCollision = true;
                    }
                }
            }
        }
        if (foundCollision) output.push_back({ i, ithNearestCollision });
    }
    if (output.size() > 0) output.push_back({ ncIndex, nearestCollision });
    return output;
}
vector<pair<size_t, Vector2f>> SensorCollider::findNearestCollision(Vector2f& cPosition, Vector2f& fPosition, SurfaceType type, 
    vector<pair<PlatformPtr, vector<Segment>>>& segmentList, float xMin, float xMax, float yMin, float yMax) const {

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
    pair<Line, Line> lines = { lineA, lineB };
    return findNearestWithinBounds(Constraint(searchBounds, lines), type, segmentList);
}

vector<pair<size_t, Vector2f>> SensorCollider::findNearestSurface(Vector2f& fPosition, SurfaceType type, 
    vector<pair<PlatformPtr, vector<Segment>>>& segmentList) const {
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
    pair<Line, Line> lines = { Line(1,0,-FLT_MAX), Line(1,0,FLT_MAX) };
    return findNearestWithinBounds(Constraint(searchBounds, lines), type, segmentList);
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

// Constraints Definitions //
// ======================= //

SensorCollider::Constraint::Constraint(FloatRect& bounds, pair<Line, Line>& lines) : bounds(bounds), lines(lines) {}
