#include "stdafx.h"
#include <math.h>
#include <algorithm>
#include <sstream>
#include "LineUtility.h"
#include "VectorUtility.h"
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

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
        if (qpxs = 0) {
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
        if (t >= 0 && t <= 1 && u >= 0 && u <= 1) return{ true, t };
        else return{ false, 0.f };
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
    return p1 + t*(p2 - p1);
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