#include "stdafx.h"
#include "Polygon.h"
#include "RectUtility.h"
#include "VectorUtility.h"
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include "GameInfo.h"
#include <sstream>

using namespace sf;
using namespace CustomUtilities;
using namespace std;

// ======================= //
// Polygon Implementations //
// ======================= //

Polygon::Polygon(const vector<Vector2f>& points, const Transform& T) {
    for (auto& p : points) {
        this->points.push_back(T.transformPoint(p));
    }
    float minX = this->points[0].x, minY = this->points[0].y, maxX = this->points[0].x, maxY = this->points[0].y;
    for (auto& p : this->points) {
        minX = min(p.x, minX);
        minY = min(p.y, minY);
        maxX = max(p.x, maxX);
        maxY = max(p.y, maxY);
    }
    maxBounds = FloatRect(minX, minY, maxX - minX, maxY - minY);

    //Populate quadtree.
    quadtree = PolygonalQuadtree(maxBounds, 0);
    for (size_t i = 0; i < this->points.size(); ++i) {
        size_t j = (i + 1) % this->points.size();
        quadtree.insert(Segment(this->points[i], this->points[j]));
    }
}
Polygon::Polygon(const vector<Vector2f>& points) : Polygon(points, Transform::Identity){}

Polygon::Polygon(const FloatRect& rect) : Polygon(toPoints(rect), Transform::Identity) {}

bool Polygon::containsPoint(const Vector2f& v) const {
    //Detect internal points by calculating the winding number of the shape given this point.
    /*int windingNumber = 0;
    for (size_t i = 0; i < points.size(); ++i) {
        //Detect intersections between this shape's edges and a horizontal line originating from v.
        Vector2f p = points[i], q = points[(i + 1) % points.size()];
        //p.y *= -1; q.y *= -1;
        if (q.y > p.y) {
            //Upward edge, so omit intersections exactly with q.
            if (v.y >= q.y || v.y < p.y) continue;
            float ix = (v.y - p.y) * (q.x - p.x) / (q.y - p.y) + p.x;
            if (v.x < ix) ++windingNumber;
        }
        else if (p.y > q.y) {
            //Downward edge, so omit start point from potential intersection.
            if (v.y >= p.y || v.y < q.y) continue;
            float ix = (v.y - p.y) * (q.x - p.x) / (q.y - p.y) + p.x;
            if (v.x < ix) --windingNumber;
        }
    }
    return windingNumber != 0;*/
    if (!maxBounds.contains(v)) return false;
    else return quadtree.getWindingNumber(v) != 0;
}

pair<bool, float> Polygon::intersects(const Segment& line) const {
    /*bool hasIntersection = false;
    float t = 1;
    for (size_t i = 0; i < points.size(); ++i) {
        Segment l(points[i], points[(i + 1) % points.size()]);
        pair<bool, float> intersection = line.intersects(l);
        if (intersection.first) {
            hasIntersection = true;
            t = min(t, intersection.second);
        }
    }
    return{ hasIntersection, t };*/
    return quadtree.intersects(line);
}

pair<bool, Segment> Polygon::findInnerLine(const Segment& line) const {
    bool containsStart = containsPoint(line.start()), containsEnd = containsPoint(line.end());
    if (containsStart && containsEnd) return{ true, line };
    else if (containsStart) {
        pair<bool, float> intersection = intersects(line);
        //if (intersection.first) cout << 1 << endl;
        return{ intersection.first, Segment(line.start(), line.atPoint(intersection.second)) };
    }
    else if (containsEnd) {
        pair<bool, float> intersection = intersects(line);
        //if (intersection.first) cout << 2 << endl;
        return{ intersection.first, Segment(line.atPoint(intersection.second), line.end()) };
    }
    else {
        Segment reverse = Segment(line.end(), line.start());
        pair<bool, float> intersection1 = intersects(line);
        pair<bool, float> intersection2 = intersects(reverse);
        //if (intersection1.first && intersection2.first) cout << 3 << " " << line.toString() << endl;
        return{ intersection1.first && intersection2.first,
            Segment(line.atPoint(intersection1.second), reverse.atPoint(intersection2.second)) };
    }
}

vector<Vector2f> Polygon::findInternalPoints(const vector<Segment>& lines) const {
    vector<Vector2f> output;
    for (auto& line : lines) {
        pair<bool, Segment> innerLine = findInnerLine(line);
        if (innerLine.first) {
            output.push_back(innerLine.second.start());
            output.push_back(innerLine.second.end());
        }
    }
    return output;
}

vector<Segment> Polygon::findInternalLines(const vector<Segment>& lines) const {
    vector<Segment> output;
    for (auto& line : lines) { 
        pair<bool, Segment> innerLine = findInnerLine(line);
        if (innerLine.first) output.push_back(innerLine.second);
    }
    return output;
}

vector<Segment> Polygon::findInternalLines(const sf::FloatRect& rect) const {
    vector<Vector2f> rectPoints = toPoints(rect);
    vector<Segment> output;
    for (size_t i = 0; i < rectPoints.size(); ++i) {
        size_t j = (i + 1) % rectPoints.size();
        pair<bool, Segment> innerLine = findInnerLine(Segment(rectPoints[i], rectPoints[j]));
        if (innerLine.first) output.push_back(innerLine.second);
    }
    return output;
}

vector<Segment> Polygon::findOverlappingLines(Polygon& shape) const {
    return quadtree.findOverlappingLines(shape);
}

Polygon Polygon::transform(const Transform& T) const {
    return Polygon(points, T);
}
void Polygon::draw(RenderTarget& target, RenderStates states, const Color& color) const {
    float ppu = GameInfo::pixelsPerUnit;
    VertexArray va = VertexArray(LinesStrip);
    for (size_t i = 0; i < points.size(); ++i) va.append(Vertex(points[i] * ppu, color));
    va.append(va[0]);
    target.draw(va, states);
}


void Polygon::draw(RenderTarget& target, RenderStates states) const {
    draw(target, states, Color::White);
}

void Polygon::drawTree(RenderTarget& target, RenderStates states) const {
    quadtree.draw(target, states);
}

FloatRect Polygon::bounds() const {
    return maxBounds;
}
string Polygon::toString() const {
    stringstream ss;
    ss << "Polygon:[";
    for (size_t i = 0; i < points.size(); ++i) {
        const Vector2f& point = points[i];
        ss << CustomUtilities::toString(point);
        if (i < points.size() - 1) ss << ",";
        else ss << "]";
    }
    return ss.str();
}

ostream& operator<<(ostream& os, const Polygon& shape) {
    return os << shape.toString();
}

// ================================= //
// PolygonalQuadtree Implementations //
// ================================= //

Polygon::PolygonalQuadtree::PolygonalQuadtree(FloatRect bounds, size_t depth) : bounds(bounds), depth(depth) {}
Polygon::PolygonalQuadtree::PolygonalQuadtree() : PolygonalQuadtree(FloatRect()){}

void Polygon::PolygonalQuadtree::clear() {
    elements = {};
    for (size_t i = 0; i < 4; ++i) {
        if (nodes[i]) {
            nodes[i]->clear();
            nodes[i] = nullptr;
        }
    }
}

void Polygon::PolygonalQuadtree::split() {
    float subWidth = bounds.width / 2;
    float subHeight = bounds.height / 2;
    float x = bounds.left, y = bounds.top;
    nodes[0] = new PolygonalQuadtree(FloatRect(x, y, subWidth, subHeight), depth + 1);
    nodes[1] = new PolygonalQuadtree(FloatRect(x + subWidth, y, subWidth, subHeight), depth + 1);
    nodes[2] = new PolygonalQuadtree(FloatRect(x, y + subHeight, subWidth, subHeight), depth + 1);
    nodes[3] = new PolygonalQuadtree(FloatRect(x + subWidth, y + subHeight, subWidth, subHeight), depth + 1);
}

int Polygon::PolygonalQuadtree::find(FloatRect& rect) const {
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

FloatRect Polygon::PolygonalQuadtree::getInnerBounds(int index) const{
    float subWidth = bounds.width / 2;
    float subHeight = bounds.height / 2;
    float x = bounds.left, y = bounds.top;
    if (index == 0) return FloatRect(x, y, subWidth, subHeight);
    else if (index == 1) return FloatRect(x + subWidth, y, subWidth, subHeight);
    else if (index == 2) return FloatRect(x, y + subHeight, subWidth, subHeight);
    else return FloatRect(x + subWidth, y + subHeight, subWidth, subHeight);
}

void Polygon::PolygonalQuadtree::insert(Segment& line) {
    //Recursive case: element is inserted at a deeper node.
    if (hasInner) {
        int index = find(line.bounds());
        if (index != -1) {
            if (!nodes[index]) {
                //Make a new inner Quadtree at this index.
                nodes[index] = new PolygonalQuadtree(getInnerBounds(index), depth + 1);
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
        for (auto& element : elements) {
            int i = find(element.bounds());
            if (i != -1) {
                if (!nodes[i]) nodes[i] = new PolygonalQuadtree(getInnerBounds(i), depth + 1);
                nodes[i]->insert(element);
                hasInner = true;
            }
            else newElements.push_back(element);
        }
        //Overwrite element list if elements have been moved to inner quadtrees.
        if (hasInner) elements = newElements;
    }

}

pair<bool, float> Polygon::PolygonalQuadtree::intersects(const Segment& line) const {
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
        pair<bool, float> intersection = nodes[i]->intersects(line);
        if (intersection.first) {
            t = min(t, intersection.second);
            hasIntersection = true;
        }
    }
    else {
        for (auto& node : nodes) {
            if (node) {
                pair<bool, float> intersection = node->intersects(line);
                if (intersection.first) {
                    t = min(t, intersection.second);
                    hasIntersection = true;
                }
            }
        }
    }

    return{ hasIntersection , t };

}

vector<Segment> Polygon::PolygonalQuadtree::findOverlappingLines(Polygon& shape) const {
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
        vector<Segment> foundLines = node->findOverlappingLines(shape);
        output.insert(output.end(), foundLines.begin(), foundLines.end());
    }
    return output;
}

int Polygon::PolygonalQuadtree::getWindingNumber(const Vector2f& v) const {
    //Count winding number from elements stored at this node.
    int windingNumber = 0;
    for (auto& element : elements) {
        //Detect intersections between this shape's edges and a horizontal line originating from v.
        Vector2f p = element.start(), q = element.end();
        if (q.y > p.y) {
            //Upward edge, so omit intersections exactly with q.
            if (v.y >= q.y || v.y < p.y) continue;
            float ix = (v.y - p.y) * (q.x - p.x) / (q.y - p.y) + p.x;
            if (v.x < ix) ++windingNumber;
        }
        else if (p.y > q.y) {
            //Downward edge, so omit start point from potential intersection.
            if (v.y >= p.y || v.y < q.y) continue;
            float ix = (v.y - p.y) * (q.x - p.x) / (q.y - p.y) + p.x;
            if (v.x < ix) --windingNumber;
        }
    }
    //Recurse on all nodes that can contain a point w such that w.x >= x and w.y == v.y.
    for (auto& node : nodes) {
        if (node) {
            if (node->bounds.top <= v.y && node->bounds.top + node->bounds.height >= v.y && node->bounds.left + node->bounds.width >= v.x) {
                windingNumber += node->getWindingNumber(v);
            }
        }
    }

    return windingNumber;
}
void Polygon::PolygonalQuadtree::draw(RenderTarget& target, RenderStates states) const {
    if (drawColor != Color::Black) CustomUtilities::draw(bounds, drawColor, target, states);
    //CustomUtilities::draw(bounds, Color::Blue, target, states);
    drawColor = Color::Black;
    float t = (maxDepth - depth)*1.f / maxDepth;
    //for (Line element : elements) CustomUtilities::draw(element.bounds(), Color(0, 255*(1-t), 255*t), target, states);
    for (auto& node : nodes) {
        if (node) node->draw(target, states);
    }
}