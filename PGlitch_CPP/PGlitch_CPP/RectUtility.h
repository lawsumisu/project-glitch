#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Color.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <set>

namespace CustomUtilities {

    template <typename T> std::string toString(const sf::Rect<T>& R) {
        std::stringstream ss;
        ss << "Rect:origin = (" << R.left << "," << R.top << "); width = "
            << R.width << "; height = " << R.height;
        return ss.str();
    }
    /// <summary>
    /// Creates a new <see cref="FloatRect"/> that represents a rectangle centered at
    /// 'center' with width and height given by 'dimensions.x' and 'dimensions.y' respectively.
    /// </summary>
    /// <param name="center"></param>
    /// <param name="dimensions"></param>
    /// <returns></returns>
    sf::FloatRect construct(const sf::Vector2f& center, const sf::Vector2f& dimensions);
    template <typename T> sf::Rect<T> copy(sf::Rect<T> R) {
        return sf::Rect<T>(R);
    }

    /// <summary>
    /// Gets the area of the input <see cref="Rect"/>.
    /// </summary>
    template <typename T> T area(sf::Rect<T> R) {
        return R.width*R.height;
    }

    std::vector<sf::Vector2f> toPoints(const sf::FloatRect& rect);
    /// <summary>
    /// Draws an axis-aligned rectangle defined by the input <see cref="FloatRect"/>.
    /// </summary>
    void draw(const sf::FloatRect& R, const sf::Color& color, sf::RenderTarget& target, sf::RenderStates states);
    void draw(const sf::FloatRect& R, const sf::Transform& T, const sf::Color& color,
        sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default);

    sf::FloatRect operator*(const sf::FloatRect& R, float f);
    sf::FloatRect operator+(const sf::FloatRect& R, sf::Vector2f& v);
    void operator+= (sf::FloatRect& rect, sf::Vector2f& v);

}

