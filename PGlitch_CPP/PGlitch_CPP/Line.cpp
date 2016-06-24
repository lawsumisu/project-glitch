#include "stdafx.h"
#include "Line.h"

using namespace sf;
using namespace std;
using namespace CustomUtilities;

Line::Line(float a, float b, float c) : a(a), b(b), c(c){}
Line::Line(Vector2f& p1, Vector2f& p2) : a(p2.y - p1.y), b(p1.x - p2.x), c(p1.x*p2.y - p2.x*p1.y) {}

Line Line::toLine(Segment& segment) {
    return Line(segment.start(), segment.end());
}

int Line::sign(Vector2f& v) const {
    float axby = a*v.x + b*v.y;
    if (axby < c) return -1;
    else if (axby > c) return 1;
    else return 0;
}

Line Line::getParallelLineTo(Vector2f& v) const {
    return Line(a, b, a*v.x + b*v.y);
}
float Line::A() { return a; }

float Line::B() { return b; }

float Line::C() { return c; }