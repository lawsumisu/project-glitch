#include "stdafx.h"
#include <math.h>
#include <algorithm>
#include <sstream>
#include "LineUtility.h"
#include "VectorUtility.h"
#include "MathUtility.h"
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <unordered_set>

using namespace CustomUtilities;
using namespace sf;
using namespace std;

Line::Line(const Vector2f& p1, const Vector2f& p2) {
    this->p1 = p1;
    this->p2 = p2;
}

Line::Line(const Vector2f& origin, float angle, float distance) :
    Line(origin, origin + Vector2f(cosf(angle), sinf(angle))*distance) {
}

pair<bool, float> Line::intersects(const Line& line) const{
    Vector2f r = p2 - p1;
    Vector2f s = line.p2 - line.p1;

    float qpxs = cross(line.p1 - p1, s);
    float pqxr = cross(p1 - line.p1, r);
    float rxs = cross(r, s);

    if (rxs == 0) {
        if (qpxs == 0) {
            //Lines are collinear
            float t0 = dot((line.p1 - p1), r) / dot(r, r);
            float t1 = dot(line.p1 + s - p1, r) / dot(r, r);

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
        if (t >= 0 && t <= 1 && u >= 0 && u <= 1) return{ true, t };
        else return{ false, 0.f };
    }

}

pair<bool, float> Line::intersects(const FloatRect& rect) const {
    if (max(p1.x, p2.x) <= rect.left || min(p1.x, p2.x) >= rect.left + rect.width ||
        max(p1.y, p2.y) <= rect.top || min(p1.y, p2.y) >= rect.top + rect.height) {
        return{ false, 0.f };
    }

    else {
        bool foundIntersect = false;
        float t = 1;
        Line l1 = Line(Vector2f(rect.left, rect.top), Vector2f(rect.left + rect.width, rect.top));
        Line l2 = Line(l1.end(), Vector2f(rect.left + rect.width, rect.top + rect.height));
        Line l3 = Line(l2.end(), Vector2f(rect.left, rect.top + rect.height));
        Line l4 = Line(l3.end(), l1.start());
        vector<Line> lines = { l1,l2,l3,l4 };

        for (const Line& L : lines) {
            pair<bool, float> intersection = intersects(L);
            if (intersection.first) {
                foundIntersect = true;
                t = min(t, intersection.second);
            }
        }
        return{ foundIntersect, t };
    }

}

unordered_set<Vector2f> Line::findAllIntersections(const FloatRect& rect) const {
    if (max(p1.x, p2.x) < rect.left || min(p1.x, p2.x) > rect.left + rect.width ||
        max(p1.y, p2.y) < rect.top || min(p1.y, p2.y) > rect.top + rect.height) {
        return {};
    }

    else {
        unordered_set<Vector2f> output = {};
        Line l1 = Line(Vector2f(rect.left, rect.top), Vector2f(rect.left + rect.width, rect.top));
        Line l2 = Line(l1.end(), Vector2f(rect.left + rect.width, rect.top + rect.height));
        Line l3 = Line(l2.end(), Vector2f(rect.left, rect.top + rect.height));
        Line l4 = Line(l3.end(), l1.start());
        vector<Line> lines = { l1,l2,l3,l4 };

        for (const Line& L : lines) {
            pair<bool, float> intersection = intersects(L);
            if (intersection.first) {
                output.insert(atPoint(intersection.second));
            }
        }
        return output;
    }
}

unordered_set<Vector2f> Line::findInteriorPoints(const FloatRect& rect) const {
    unordered_set<Vector2f> output = {};
    if (rect.contains(p1)) output.insert(p1);
    if (rect.contains(p2)) output.insert(p2);

    return output;
}

pair<bool, Line> Line::findInnerLine(const FloatRect& rect) const {
    if (rect.contains(p1) && rect.contains(p2)) return{ true, *this };
    else if (rect.contains(p1)){    
        pair<bool, float> intersection = intersects(rect);
        return{ intersection.first, Line(p1, atPoint(intersection.second)) };
    }
    else if (rect.contains(p2)) {
        pair<bool, float> intersection = intersects(rect);
        return{ intersection.first, Line(atPoint(intersection.second), p2) };
    }
    else {
        Line reverse = Line(p2, p1);
        pair<bool, float> intersection1 = intersects(rect);
        pair<bool, float> intersection2 = reverse.intersects(rect);
        return{ intersection1.first && intersection2.first,
            Line(atPoint(intersection1.second), reverse.atPoint(intersection2.second)) };
    }
}
string Line::toString() const {
    stringstream ss;
    ss << "Line:[(" << p1.x << "," << p1.y << ") -> (" << p2.x << "," << p2.y << ")]";
    return ss.str();
}

void Line::draw(const Color& color, sf::RenderTarget& target, sf::RenderStates states) const {
    VertexArray va = VertexArray(sf::Lines, 2);
    va[0] = p1;
    va[1] = p2;

    va[0].color = color;
    va[1].color = color;

    target.draw(va, states);
        
}

float Line::cross(Vector2f& v1, Vector2f& v2) {
    return (v1.x*v2.y) - (v1.y*v2.x);
}

sf::Vector2f Line::atPoint(float t) const {
    Vector2f v = p1 + t*(p2 - p1);

    //Due to potential rounding errors of t, 
    int precision = 100000;
    //v.x = roundf(v.x*precision) / precision;
    //v.y = roundf(v.y*precision) / precision;
    return v;
}

float Line::getT(Vector2f& v) const {
    float tx = (v - p1).x / (p2 - p1).x;
    float ty = (v - p1).y / (p2 - p1).y;

    if ((p2-p1).x != 0) return tx;
    else return ty;
}

Line Line::moveP1(const Vector2f& v) const {
    Vector2f diff = v - p1;
    return Line(v, p2 + diff);
}

Line CustomUtilities::operator*(const Line& line, float f) {
    return Line(line.p1 *f, line.p2*f);
}