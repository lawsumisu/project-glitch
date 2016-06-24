#pragma once

#include <vector>
#include <SFML/Graphics/Rect.hpp>
#include "LineUtility.h"
#include <SFML/Graphics/Drawable.hpp>
#include "Enumerators.h"
#include "Polygon.h"

/// <summary>
/// Data structure for storing Lines for a FreeformCollider. Use this object for collisions between rectangles and 
/// stored lines.
/// </summary>
class Quadtree : public sf::Drawable{
private:
    //Fields
    size_t maxElements = 10, maxDepth = 5;

    size_t depth;
    bool hasInner = false;
    mutable bool setColor = false;
    mutable sf::Color drawColor;
    std::vector<CustomUtilities::Segment> elements = {};
    sf::FloatRect bounds;
    std::vector<Quadtree*> nodes = {nullptr, nullptr, nullptr, nullptr};

    //Methods

    void retrieve(sf::FloatRect& rect, std::vector<CustomUtilities::Segment>& output) const;

    sf::Vector2f getEffectivePoint(CustomUtilities::Segment& line, SurfaceType type) const;

    void findInternalPoints(Polygon& shape, std::vector<sf::Vector2f>& foundPoints) const;

    std::vector<size_t> findInner(sf::FloatRect& rect) const;

    void updateDrawColor(SurfaceType type) const;
public:
    //Constructor
    Quadtree();
    Quadtree(sf::FloatRect bounds, size_t depth);

    //Methods

    /// <summary>
    /// Clears the tree.
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
    sf::FloatRect getInnerBounds(int index);

    int find(sf::FloatRect& rect) const;

    void insert(CustomUtilities::Segment& line);

    std::vector<CustomUtilities::Segment> retrieve(sf::FloatRect& rect) const;

    std::pair<bool, sf::Vector2f> findIntersections(sf::FloatRect& rect, SurfaceType type) const;

    std::vector<CustomUtilities::Segment> findInternalLines(Polygon& shape) const;

    std::pair<bool, float> findIntersection(CustomUtilities::Segment& line) const;

    void draw(sf::RenderTarget& target, sf::RenderStates = sf::RenderStates::Default) const ;
};



