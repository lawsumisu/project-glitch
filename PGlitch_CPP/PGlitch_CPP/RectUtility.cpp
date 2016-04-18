#include "stdafx.h"
#include "RectUtility.h"
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

using namespace CustomUtilities;

FloatRect CustomUtilities::construct(Vector2f& center, Vector2f& dimensions) {
    return FloatRect(center.x - dimensions.x / 2, center.y - dimensions.y / 2, dimensions.x, dimensions.y);
}

void CustomUtilities::draw(const FloatRect& R, const Color& color, sf::RenderTarget& target, sf::RenderStates states) {
    VertexArray va = VertexArray(sf::LinesStrip, 5);
    va[0] = Vector2f(R.left, R.top);
    va[1] = Vector2f(R.left + R.width, R.top);
    va[2] = Vector2f(R.left + R.width, R.top + R.height);
    va[3] = Vector2f(R.left, R.top + R.height);
    va[4] = va[0];

    for (size_t i = 0; i < va.getVertexCount(); ++i) va[i].color = color;

    target.draw(va, states);
}

FloatRect CustomUtilities::operator*(const FloatRect& R, float f) {
    return FloatRect(R.left * f, R.top*f, R.width*f, R.height*f);
}