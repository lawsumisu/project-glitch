#include "stdafx.h"
#include "Quadtree.h"
#include "RectUtility.h"

using namespace std;
using namespace sf;
using namespace CustomUtilities;

Quadtree::Quadtree(FloatRect bounds, size_t depth) : bounds(bounds), depth(depth) {}

Quadtree::Quadtree() : Quadtree(FloatRect(), 0) {}
void Quadtree::clear() {
    elements = {};
    for (size_t i = 0; i < 4; ++i) {
        if (nodes[i]) {
            nodes[i]->clear();
            nodes[i] = nullptr;
        }
    }
}

void Quadtree::split() {
    float subWidth = bounds.width / 2;
    float subHeight = bounds.height / 2;
    float x = bounds.left, y = bounds.top;
    nodes[0] = new Quadtree(FloatRect(x, y, subWidth, subHeight), depth + 1);
    nodes[1] = new Quadtree(FloatRect(x + subWidth, y, subWidth, subHeight), depth + 1);
    nodes[2] = new Quadtree(FloatRect(x, y + subHeight, subWidth, subHeight), depth + 1);
    nodes[3] = new Quadtree(FloatRect(x + subWidth, y + subHeight, subWidth, subHeight), depth + 1);
}

FloatRect Quadtree::getInnerBounds(int index) {
    float subWidth = bounds.width / 2;
    float subHeight = bounds.height / 2;
    float x = bounds.left, y = bounds.top;
    if (index == 0) return FloatRect(x, y, subWidth, subHeight);
    else if (index == 1) return FloatRect(x + subWidth, y, subWidth, subHeight);
    else if (index == 2) return FloatRect(x, y + subHeight, subWidth, subHeight);
    else return FloatRect(x + subWidth, y + subHeight, subWidth, subHeight);
}

vector<size_t> Quadtree::findInner(FloatRect& rect) const {
    if (!nodes[0]) return{};
    vector<size_t> output;

    for (size_t i = 0; i < 4; ++i) {
        if (rect.intersects(nodes[i]->bounds)) output.push_back(i);
    }
    return output;
}
int Quadtree::find(FloatRect& rect) const {
    double verticalMidpoint = bounds.left + bounds.width / 2;
    double horizontalMidpoint = bounds.top + bounds.height / 2;

    // Object can completely fit within the top quadrants
    bool topQuadrant = (rect.top + rect.height < horizontalMidpoint);
    // Object can completely fit within the bottom quadrants
    bool bottomQuadrant = (rect.top > horizontalMidpoint);

    // Object can completely fit within the left quadrants
    if (rect.left + rect.width < verticalMidpoint) {
        if (topQuadrant) return 0;
        else if (bottomQuadrant) return 2;
    }
    // Object can completely fit within the right quadrants
    else if (rect.left > verticalMidpoint) {
        if (topQuadrant) return 1;
        else if (bottomQuadrant) return 3;
    }
    return -1;
}

void Quadtree::insert(Segment& line) {
    //Recursive case: element is inserted at a deeper node.
    if (hasInner) {
        int index = find(line.bounds());
        if (index != -1) {
            if (!nodes[index]) {
                //Make a new inner Quadtree at this index.
                nodes[index] = new Quadtree(getInnerBounds(index), depth + 1);
            }
            nodes[index]->insert(line);
        }
        else elements.push_back(line);
    }
    else {
        elements.push_back(line);
    }
    //Now, need to check if elements is at maximum capacity.
    if (!hasInner && elements.size() > maxElements && depth < maxDepth) {
        //Populate new nodes with elements.
        vector<Segment> newElements = {};
        for (Segment element : elements) {
            int i = find(element.bounds());
            if (i != -1) {
                if (!nodes[i]) nodes[i] = new Quadtree(getInnerBounds(i), depth + 1);
                nodes[i]->insert(element);
                hasInner = true;
            }
            else newElements.push_back(element);
        }
        //Overwrite element list if elements have been moved to inner quadtrees.
        if (hasInner) elements = newElements;
    }
    
}

Vector2f Quadtree::getEffectivePoint(Segment& line, SurfaceType type) const {
    if ((type == SurfaceType::GROUND && line.start().y < line.end().y) || 
        (type == SurfaceType::CEILING && line.start().y > line.end().y) ||
        (type == SurfaceType::RIGHT && line.start().x < line.end().x) ||
        (type == SurfaceType::LEFT && line.start().x > line.end().x)) {
        return line.start();
    }
    else return line.end();
}
pair<bool, Vector2f> Quadtree::findIntersections(FloatRect& rect, SurfaceType type) const {
    //Based on the type of collision, use a rect that is a combination of the input rect's dimensions and the boundaries of this quadtree.
    FloatRect effectiveRect = rect;
    vector<size_t> ordering = {};
    
    if (type == SurfaceType::GROUND) {
        effectiveRect = FloatRect(rect.left, bounds.top, rect.width, rect.top - bounds.top + rect.height);
        //Ordering: left->right, top->bottom.
        ordering = { 0, 1, 2, 3 };
        //cout << toString(effectiveRect) << endl;
    }
    else if (type == SurfaceType::CEILING) {
        effectiveRect = FloatRect(rect.left, rect.top, rect.width, bounds.top + bounds.height - rect.top);
        //Ordering: left->right, bottom->top.
        ordering = { 2, 3, 0, 1 };
    }
    else if (type == SurfaceType::LEFT) {
        effectiveRect = FloatRect(rect.left, rect.top, bounds.left + bounds.width - rect.left, rect.height);
        //Ordering: top->bottom, right->left.
        ordering = { 1,3,0,2 };
    }
    else if (type == SurfaceType::RIGHT) {
        effectiveRect = FloatRect(bounds.left, rect.top, rect.left + rect.width - bounds.left, rect.height);
        //Ordering: top->bottom, left->right.
        ordering = { 0,2,1,3 };
    }
    bool foundIntersection = false;
    Vector2f bestIntersection;
    //First, check for collisions with elements in this quadtree.
    for (auto& element : elements) {
        pair<bool, Segment> innerLine = element.findInnerLine(effectiveRect);
        if (innerLine.first) {
            setColor = true;
            //Update best intersection.
            Vector2f p = getEffectivePoint(innerLine.second, type);
            if (!foundIntersection ||
                (type == SurfaceType::GROUND && p.y < bestIntersection.y) ||
                (type == SurfaceType::CEILING && p.y > bestIntersection.y) ||
                (type == SurfaceType::RIGHT && p.x < bestIntersection.x) ||
                (type == SurfaceType::LEFT && p.x > bestIntersection.x)) {
                bestIntersection = p;
                foundIntersection = true;
            }
        }
    }
    if (foundIntersection) updateDrawColor(type);
    bool foundInnerIntersection = false;
    //Now check inner quadtrees, in the specified ordering.
    if (hasInner) {
        for (size_t i = 0; i <= 3; ++i) {
            Quadtree* innerQT = nodes[i];
            if (innerQT) {
                pair<bool, Vector2f> intersection = innerQT->findIntersections(rect, type);
                if (intersection.first) {
                    foundInnerIntersection = true;

                    //Update best intersection.
                    Vector2f p = intersection.second;
                    if (!foundIntersection ||
                        (type == SurfaceType::GROUND && p.y < bestIntersection.y) ||
                        (type == SurfaceType::CEILING && p.y > bestIntersection.y) ||
                        (type == SurfaceType::RIGHT && p.x < bestIntersection.x) ||
                        (type == SurfaceType::LEFT && p.x > bestIntersection.x)) {
                        bestIntersection = p;
                        foundIntersection = true;
                    }
                }
            }
            //Can break at halfway point if found intersections already.
            if (i == 1 && foundInnerIntersection) break;
        }
    }   
    return{ foundIntersection, bestIntersection };
}

pair<bool, float> Quadtree::findIntersection(Segment& line) const {
    //Check for intersections with all elements in this quadtree.
    bool hasIntersection = false;
    float t = 1;
    for (auto& element : elements) {
        pair<bool, float> intersection = line.intersects(element);
        if (intersection.first) {
            t = min(t, intersection.second);
            hasIntersection = true;
        }
    }
    //Recursive case
    int i = find(line.bounds());
    if (i != -1 && nodes[i]) {
        pair<bool, float> intersection = nodes[i]->findIntersection(line);
        if (intersection.first) {
            t = min(t, intersection.second);
            hasIntersection = true;
        }
    }
    else {
        for (auto& node : nodes) {
            if (node) {
                pair<bool, float> intersection = node->findIntersection(line);
                if (intersection.first) {
                    t = min(t, intersection.second);
                    hasIntersection = true;
                }
            }
        }
    }

    return{ hasIntersection , t };
    
}

void Quadtree::findInternalPoints(Polygon& shape, vector<Vector2f>& foundPoints) const {
    //Check for internal points with lines in this quadtree.
    vector<Vector2f> output = shape.findInternalPoints(elements);
    foundPoints.insert(foundPoints.end(), output.begin(), output.end());

    //Now do recursive check.
    for (auto& node : nodes) {
        if (!node || !node->bounds.intersects(shape.bounds())) continue;
        vector<Vector2f> boundPoints = toPoints(node->bounds);
        for (auto& p : boundPoints) {
            if (shape.containsPoint(p)) {
                node->findInternalLines(shape);
                break;
            }
        }
    }
}

vector<Segment> Quadtree::findInternalLines(Polygon& shape) const {
    if (!bounds.intersects(shape.bounds())) return{};

    //Check for internal points with lines in this quadtree.
    vector<Segment> output = shape.findInternalLines(elements);
    if (output.size() > 0) {
        setColor = true;
        drawColor = Color::Red;
        //for (auto& s : output) cout << s.toString() << ", " << output.size() << " " <<  endl;
    }

    //Now do recursive check.
    for (auto& node : nodes) {
        if (!node || !node->bounds.intersects(shape.bounds())) continue;
        vector<Segment> foundLines = node->findInternalLines(shape);
        output.insert(output.end(), foundLines.begin(), foundLines.end());
    }
    return output;
}
void Quadtree::retrieve(FloatRect& rect, vector<Segment>& output) const {
    int index = find(rect);
    if (index != -1) nodes[index]->retrieve(rect, output);
    else {
        output.insert(output.end(), elements.begin(), elements.end());
        //Add lines from relevant nodes.
        vector<size_t> nodeIndices = findInner(rect);
        for (size_t i : nodeIndices) nodes[i]->retrieve(rect, output);
    }
}
vector<Segment> Quadtree::retrieve(FloatRect& rect) const {
    vector<Segment> output = {};
    retrieve(rect, output);
    return output;
}

void Quadtree::updateDrawColor(SurfaceType type) const {
    switch (type) {
    case SurfaceType::GROUND: drawColor.g += 128; break;
    case SurfaceType::LEFT: drawColor.b += 128; break;
    case SurfaceType::RIGHT: drawColor.r += 128; break;
    case SurfaceType::CEILING:
        drawColor.g += 127;
        drawColor.r += 127;
        drawColor.b += 127;
        break;
    default: break;
    }
}
void Quadtree::draw(RenderTarget& target, RenderStates states) const {
    if (drawColor != Color::Black) CustomUtilities::draw(bounds, drawColor, target, states);
    //CustomUtilities::draw(bounds, Color::Blue, target, states);
    drawColor = Color::Black;
    float t = (maxDepth - depth)*1.f / maxDepth;
    //for (Line element : elements) CustomUtilities::draw(element.bounds(), Color(0, 255*(1-t), 255*t), target, states);
    for (auto& node : nodes) {
        if (node) node->draw(target, states);
    }
}


