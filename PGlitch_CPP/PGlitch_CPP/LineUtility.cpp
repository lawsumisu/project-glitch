#include "stdafx.h"
#include <math.h>
#include <algorithm>
#include <sstream>
#include "LineUtility.h"
#include "VectorUtility.h"
#include "MathUtility.h"
#include "RectUtility.h"
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include "GameInfo.h"
#include <unordered_set>

using namespace CustomUtilities;
using namespace sf;
using namespace std;

Segment::Segment(const Vector2f& p1, const Vector2f& p2) {
    this->p = p1;
    this->q = p2;
}

Segment::Segment(const Vector2f& origin, float angle, float distance) :
    Segment(origin, origin + Vector2f(cosf(angle), sinf(angle))*distance) {
}

pair<bool, float> Segment::intersects(const Segment& line, bool isInfiniteLine) const{
    Vector2f r = q - p;
    Vector2f s = line.q - line.p;

    float qpxs = cross(line.p - p, s);
    float pqxr = cross(p - line.p, r);
    float rxs = cross(r, s);

    if (rxs == 0) {
        if (qpxs == 0) {
            //Lines are collinear
            float t0 = dot((line.p - p), r) / dot(r, r);
            float t1 = dot(line.p + s - p, r) / dot(r, r);

            float tmin = min(t0, t1);
            float tmax = max(t0, t1);

            if (tmin >= 0 && tmin <= 1) return{ true, tmin };
            else if (tmax >= 0 && tmax <= 1) return{ true, tmax };
            else return{ false, 0.f };
        }
        else {
            //Lines are parallel but not intersecting
            return{ false, 0.f };
        }
    }
    else {
        float t = qpxs / rxs;
        float u = -pqxr / rxs;
        
        //Due to potential rounding errors, need to round t.
        float precision = 100000.f;
        t = roundf(t*precision) / precision;
        if (u >= 0 && u <= 1 && (isInfiniteLine || (t >= 0 && t <= 1))) return{ true, t };
        else return{ false, 0.f };
    }

}

pair<bool, float> Segment::intersects(const FloatRect& rect) const {
    if (max(p.x, q.x) < rect.left || min(p.x, q.x) > rect.left + rect.width ||
        max(p.y, q.y) < rect.top || min(p.y, q.y) > rect.top + rect.height) {
        return{ false, 0.f };
    }

    else {
        bool foundIntersect = false;
        float t = 1;
        //Get Ax + By = C form of line.
        float a = q.y - p.y, b = p.x - q.x, c = p.x*q.y - q.x*p.y;
        vector<Vector2f> rectPoints = toPoints(rect);
        for (size_t i = 1; i <= rectPoints.size(); ++i) {
            size_t j = i%rectPoints.size();
            float s_p = a * rectPoints[i - 1].x + b * rectPoints[i - 1].y;
            float s_q = a * rectPoints[j].x + b * rectPoints[j].y;
            //Compare both values with c to see if they lie on opposites sides of this line.
            if ((s_p <= c && s_q >= c) || (s_p >= c && s_q <= c)) {
                pair<bool, float> intersection = intersects(Segment(rectPoints[i - 1], rectPoints[j]));
                foundIntersect = foundIntersect || intersection.first;
                if (intersection.first) t = min(t, intersection.second);
            }
        }
        return{ foundIntersect, t };
    }

}

unordered_set<Vector2f> Segment::findAllIntersections(const FloatRect& rect) const {
    if (max(p.x, q.x) < rect.left || min(p.x, q.x) > rect.left + rect.width ||
        max(p.y, q.y) < rect.top || min(p.y, q.y) > rect.top + rect.height) {
        return {};
    }

    else {
        unordered_set<Vector2f> output = {};
        Segment l1 = Segment(Vector2f(rect.left, rect.top), Vector2f(rect.left + rect.width, rect.top));
        Segment l2 = Segment(l1.end(), Vector2f(rect.left + rect.width, rect.top + rect.height));
        Segment l3 = Segment(l2.end(), Vector2f(rect.left, rect.top + rect.height));
        Segment l4 = Segment(l3.end(), l1.start());
        vector<Segment> lines = { l1,l2,l3,l4 };

        for (const Segment& L : lines) {
            pair<bool, float> intersection = intersects(L);
            if (intersection.first) {
                output.insert(atPoint(intersection.second));
            }
        }
        return output;
    }
}

unordered_set<Vector2f> Segment::findInteriorPoints(const FloatRect& rect) const {
    unordered_set<Vector2f> output = {};
    if (rect.contains(p)) output.insert(p);
    if (rect.contains(q)) output.insert(q);

    return output;
}

pair<bool, Segment> Segment::findInnerLine(const FloatRect& rect) const {
    if (rect.contains(p) && rect.contains(q)) return{ true, *this };
    else if (rect.contains(p)){    
        pair<bool, float> intersection = intersects(rect);
        return{ intersection.first, Segment(p, atPoint(intersection.second)) };
    }
    else if (rect.contains(q)) {
        pair<bool, float> intersection = intersects(rect);
        return{ intersection.first, Segment(atPoint(intersection.second), q) };
    }
    else {
        Segment reverse = Segment(q, p);
        pair<bool, float> intersection1 = intersects(rect);
        pair<bool, float> intersection2 = reverse.intersects(rect);
        return{ intersection1.first && intersection2.first,
            Segment(atPoint(intersection1.second), reverse.atPoint(intersection2.second)) };
    }
}

FloatRect Segment::bounds() const {
    float x = min(p.x, q.x);
    float y = min(p.y, q.y);

    return FloatRect(x, y, abs(q.x - p.x), abs(q.y - p.y));
}
bool Segment::operator==(const Segment& otherSegment) const {
    return p == otherSegment.p && q == otherSegment.q;
}
string Segment::toString() const {
    stringstream ss;
    ss << "Segment:[(" << p.x << "," << p.y << ") -> (" << q.x << "," << q.y << ")]";
    return ss.str();
}

ostream& CustomUtilities::operator<<(ostream& os, const Segment& segment) {
    return os << segment.toString();
}
void Segment::draw(const Color& color, sf::RenderTarget& target, sf::RenderStates states) const {
    float ppu = GameInfo::pixelsPerUnit;
    VertexArray va = VertexArray(sf::Lines, 2);
    va[0] = p*ppu;
    va[1] = q*ppu;

    va[0].color = color;
    va[1].color = color;

    target.draw(va, states);
        
}

float Segment::cross(Vector2f& v1, Vector2f& v2) {
    return (v1.x*v2.y) - (v1.y*v2.x);
}

sf::Vector2f Segment::atPoint(float t) const {
    Vector2f v = p + t*(q - p);

    //Due to potential rounding errors of t, 
    int precision = 100000;
    //v.x = roundf(v.x*precision) / precision;
    //v.y = roundf(v.y*precision) / precision;
    return v;
}

float Segment::getT(Vector2f& v) const {
    float tx = (v - p).x / (q - p).x;
    float ty = (v - p).y / (q - p).y;

    if ((q-p).x != 0) return tx;
    else return ty;
}

Segment Segment::transform(const Transform& T) const {
    return Segment(T.transformPoint(p), T.transformPoint(q));
}
Segment Segment::moveP1(const Vector2f& v) const {
    Vector2f diff = v - p;
    return Segment(v, q + diff);
}

Segment CustomUtilities::operator*(const Segment& line, float f) {
    return Segment(line.p *f, line.q*f);
}