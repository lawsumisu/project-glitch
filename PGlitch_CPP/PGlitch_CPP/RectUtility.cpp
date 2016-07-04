#include "stdafx.h"
#include "RectUtility.h"
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include "GameInfo.h"

using namespace CustomUtilities;
using namespace std;
using namespace sf;

FloatRect CustomUtilities::construct(const Vector2f& center, const Vector2f& dimensions) {
    return FloatRect(center.x - dimensions.x / 2, center.y - dimensions.y / 2, dimensions.x, dimensions.y);
}

vector<Vector2f> CustomUtilities::toPoints(const FloatRect& rect) {
    return { {rect.left, rect.top}, {rect.left + rect.width, rect.top},
    {rect.left + rect.width, rect.top + rect.height}, {rect.left, rect.top + rect.height} };
}

void CustomUtilities::draw(const FloatRect& R, const Color& color, sf::RenderTarget& target, sf::RenderStates states) {
    float ppu = GameInfo::pixelsPerUnit;
    VertexArray va = VertexArray(sf::LinesStrip, 5);
    va[0] = Vector2f(R.left, R.top) * ppu;
    va[1] = Vector2f(R.left + R.width, R.top) * ppu;
    va[2] = Vector2f(R.left + R.width, R.top + R.height) * ppu;
    va[3] = Vector2f(R.left, R.top + R.height) * ppu;
    va[4] = va[0];

    for (size_t i = 0; i < va.getVertexCount(); ++i) va[i].color = color;

    target.draw(va, states);
}

void CustomUtilities::draw(const FloatRect& R, const Transform& T, const Color& color,
    sf::RenderTarget& target, sf::RenderStates states) {
    float ppu = GameInfo::pixelsPerUnit;
    vector<Vector2f> points = toPoints(R);
    VertexArray va = VertexArray(sf::LinesStrip);
    for (const Vector2f& p : points) {
        va.append(Vertex(T.transformPoint(p)*ppu, color));
    }
    va.append(va[0]);

    target.draw(va, states);
}
FloatRect CustomUtilities::operator*(const FloatRect& R, float f) {
    return FloatRect(R.left * f, R.top*f, R.width*f, R.height*f);
}

FloatRect CustomUtilities::operator+(const FloatRect& R, Vector2f& v) {
    return FloatRect(R.left + v.x, R.top + v.y, R.width, R.height);
}

void CustomUtilities::operator+=(FloatRect& R, Vector2f& v) {
    R.left += v.x;
    R.top += v.y;
}


