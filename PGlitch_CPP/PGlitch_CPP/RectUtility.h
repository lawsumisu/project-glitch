#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Color.hpp>
#include <iostream>
#include <vector>
#include <set>

using namespace sf;
namespace CustomUtilities {

    template <typename T> std::string toString(const Rect<T>& R) {
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
    FloatRect construct(const Vector2f& center, const Vector2f& dimensions);
    template <typename T> Rect<T> copy(Rect<T> R) {
        return Rect<T>(R);
    }

    /// <summary>
    /// Gets the area of the input <see cref="Rect"/>.
    /// </summary>
    template <typename T> T area(Rect<T> R) {
        return R.width*R.height;
    }

    std::vector<sf::Vector2f> toPoints(const sf::FloatRect& rect);
    /// <summary>
    /// Draws an axis-aligned rectangle defined by the input <see cref="FloatRect"/>.
    /// </summary>
    void draw(const FloatRect& R, const Color& color, sf::RenderTarget& target, sf::RenderStates states);
    void draw(const FloatRect& R, const sf::Transform& T, const Color& color,
        sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default);

    FloatRect operator*(const FloatRect& R, float f);

    class RectSorter {
    private:
        //Fields

        std::vector<FloatRect> rects;
        class RValue {
        private:
            bool _startNotEnd;
            size_t index;
            float value;

        public:
            RValue(size_t index, float value, bool startNotEnd) : index(index), value(value), _startNotEnd(startNotEnd) {}

            friend bool operator<(const RValue& rp1, const RValue& rp2) {
                if (rp1.value == rp2.value) return rp1.index < rp2.index;
                else return (rp1.value < rp2.value);
            }

            bool startNotEnd() const { return _startNotEnd; }
            size_t i() const { return index; }
            float v() const { return value; }
        };

        std::vector<RValue> xIndices;
        std::vector<RValue> yIndices;

        std::vector<size_t> xStartIndices, yStartIndices, yEndIndices;
        std::vector<size_t> xEndIndices;
        std::vector<size_t> xStartToEndLinks, xEndToStartLinks;

        //Methods
        int find(float value, size_t start, size_t end, bool xNotY, bool startNotEnd) const;
        float rvalue(size_t i, bool xNotY, bool startNotEnd) const;

    public:
        //Constructor

        RectSorter(const std::vector<FloatRect>& rects);

        //Methods

        std::vector<size_t> findIntersects(const FloatRect& rect) const;
        std::string toString() const;

    };
}

