#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transform.hpp>
#include "Segment.h"
#include <vector>
#include <string>
#include <iostream>

/// <summary>
/// Represents a polygon constructed from a series of points.
/// Immutable.
/// </summary>
class Polygon : sf::Drawable {
private:
    // ============== //
    // Data Structure //
    // ============== //

    class PolygonalQuadtree : sf::Drawable {
    private:
        //Fields
        size_t maxElements = 10, maxDepth = 5;

        size_t depth;
        bool hasInner = false;
        mutable bool setColor = false;
        mutable sf::Color drawColor;
        std::vector<Segment> elements = {};
        sf::FloatRect bounds;
        std::vector<PolygonalQuadtree*> nodes = { nullptr, nullptr, nullptr, nullptr };
    public:

        //Constructor
        PolygonalQuadtree();
        PolygonalQuadtree(sf::FloatRect bounds, size_t depth = 0);

        // ======= //
        // Methods //
        // ======= //

        /// <summary>
        /// Recursively clears this quadtree of elements.
        /// </summary>
        void clear();

        /// <summary>
        /// Splits this node into 4 subnodes.
        /// </summary>
        void split();

        /// <summary>
        /// Given an index, finds the bounds of a inner Quadtree.
        /// Requires that 0 <= index <= 3.
        /// </summary>
        sf::FloatRect getInnerBounds(int index) const;

        int find(sf::FloatRect& rect) const;
        /// <summary>
        /// Inserts a segment into this quadtree.
        /// </summary>
        /// <param name="segment"></param>
        void insert(Segment& segment);

        std::pair<bool, float> intersects(const Segment& segment) const;

        /// <summary>
        /// Gets the winding number of a point within this quadtree with respect to all of its contained elements.
        /// </summary>
        /// <param name="segment"></param>
        /// <returns></returns>
        int getWindingNumber(const sf::Vector2f& v) const;

        /// <summary>
        /// Returns a subset of segments within this shape that overlap the input shape.
        /// </summary>
        std::vector<Segment> findOverlappingLines(Polygon& shape) const;

        void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const;
    };

    // ====== //
    // Fields //
    // ====== //
    
    // AF: represents points of the polygon, where points[i] and points[(i+1) % points.size()] represent the start and end points
    // of an edge between those points.
    // RI: points.size() >= 3, all points are unique.
    std::vector<sf::Vector2f> points;

    sf::FloatRect maxBounds;

    PolygonalQuadtree quadtree;

    // ======= //
    // Methods //
    // ======= //

    std::pair<bool, float> intersects(const Segment& line) const;

public:
    // ============ //
    // Constructors //
    // ============ //
    Polygon(const std::vector<sf::Vector2f>& points, const sf::Transform& T);
    Polygon(const std::vector<sf::Vector2f>& points);
    Polygon(const sf::FloatRect& rect);
    
    // ======= //
    // Methods //
    // ======= //
    
    bool containsPoint(const sf::Vector2f& v) const;

    /// <summary>
    /// Given an input line, finds the section of that line that is contained within this shape.
    /// If none of the line is contained within this shape, then output.first == false.
    /// </summary>
    /// <param name="line"></param>
    /// <returns></returns>
    std::pair<bool, Segment> findInnerLine(const Segment& line) const;

    std::vector<sf::Vector2f> findInternalPoints(const std::vector<Segment>& lines) const;
    
    std::vector<Segment> findInternalLines(const std::vector<Segment>& lines) const;
    std::vector<Segment> findInternalLines(const sf::FloatRect& rect) const;

    /// <summary>
    /// Returns a subset of segments within this shape that overlap the input shape.
    /// </summary>
    /// <param name="shape"></param>
    /// <returns></returns>
    std::vector<Segment> findOverlappingLines(Polygon& shape) const;

    /// <summary>
    /// Returns a shape that represents this shape being transformed by a transform T.
    /// </summary>
    /// <param name="T"></param>
    /// <returns></returns>
    Polygon transform(const sf::Transform& T) const;

    std::string toString() const;

    friend std::ostream& operator<<(std::ostream& os, const Polygon& shape);

    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    void draw(sf::RenderTarget& target, sf::RenderStates states, const sf::Color& color) const;

    void drawTree(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const;

    sf::FloatRect bounds() const;

};
