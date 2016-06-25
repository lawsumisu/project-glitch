#include "stdafx.h"
#include "FreeformCollider.h"
#include "LineUtility.h"
#include "RectUtility.h"
#include "VectorUtility.h"
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include "GameInfo.h"
#include <unordered_set>
#include <functional>
#include <time.h>

using namespace sf;
using namespace std;
using namespace CustomUtilities;

FreeformCollider::FreeformCollider(vector<Vector2f> points) :sorter({}), colliderShape(points) {
    this->points = points;
    //this->points.push_back(points[0]);

    float minX = points[0].x, maxX = points[0].x, minY = points[0].y, maxY = points[0].y;
    float mbMinX = points[0].x, mbMaxX = points[0].x, mbMinY = points[0].y, mbMaxY = points[0].y;
    for (size_t i = 0; i < points.size(); ++i) {
        size_t idx = (i + 1) % points.size();
        minX = min(minX, points[idx].x);
        maxX = max(maxX, points[idx].x);
        minY = min(minY, points[idx].y);
        maxY = max(maxY, points[idx].y);

        //Prepare inner bounds if necessary.
        mbMinX = min(mbMinX, points[idx].x);
        mbMaxX = max(mbMaxX, points[idx].x);
        mbMinY = min(mbMinY, points[idx].y);
        mbMaxY = max(mbMaxY, points[idx].y);

        if ((i >= 1 && i % 10 == 0) || i == points.size()-1) {
            boundsList.push_back(FloatRect(mbMinX, mbMinY, mbMaxX - mbMinX, mbMaxY - mbMinY));
            boundIndices.push_back(i);
            if (i < points.size() - 1) {
                mbMinX = points[i].x;
                mbMaxX = points[i].x;
                mbMinY = points[i].y;
                mbMaxY = points[i].y;
            }
        }
    }
    maxBounds = FloatRect(minX, minY, maxX - minX, maxY - minY);
    quadtree = Quadtree(maxBounds, 0);
    for (size_t i = 0; i < points.size(); ++i) {
        size_t j = (i + 1) % points.size();
        quadtree.insert(Segment(points[i], points[j]));
    }

    //Set up mini bounds.
    sorter = RectSorter(boundsList);

}

FreeformCollider::FreeformCollider(FloatRect rect) :
    FreeformCollider({ Vector2f(rect.left, rect.top), Vector2f(rect.left + rect.width, rect.top),
        Vector2f(rect.left + rect.width, rect.top + rect.height), Vector2f(rect.left, rect.top + rect.height) }) {}

pair<bool, float> FreeformCollider::intersects(const Transform& info, const FloatRect& rect, SurfaceType type, bool findExterior) const {
    FloatRect effectiveMaxBounds = info.transformRect(maxBounds);

    //Fail early if rect isn't even within max bounds.
    if (!effectiveMaxBounds.intersects(rect)) return{ false, 0.f };
    else {
        pair<bool, Vector2f> bestIntersection = quadtree.findIntersections(info.getInverse().transformRect(rect), type);
        if (bestIntersection.first) {
            //Need to transform point back into world space.
            Vector2f p = info.transformPoint(bestIntersection.second);
            if (type == SurfaceType::GROUND || type == SurfaceType::CEILING) {
                //if (type == SurfaceType::GROUND) cout << "Collision at " << toString(p) << endl;
                return{ true, p.y };
            }
            else return{ true, p.x };
        }
        else return{ false, 0.f };



        /*bool foundIntersection = false;
        bool isLegitimate = false;
        Vector2f bestIntersection;
        FloatRect initialEffectiveRect;
        if (type == SurfaceType::GROUND || type == SurfaceType::CEILING) {
            initialEffectiveRect = FloatRect(rect.left, effectiveMaxBounds.top, rect.width, effectiveMaxBounds.height);
        }
        else {
            initialEffectiveRect = FloatRect(effectiveMaxBounds.left, rect.top, effectiveMaxBounds.width, rect.height);
        }
        //if (type == SurfaceType::LEFT) cout << toString(initialEffectiveRect) << endl;
        vector<size_t> rectIndices = sorter.findIntersects(info.getInverse().transformRect(initialEffectiveRect));
        //if (type == SurfaceType::GROUND) cout << rectIndices.size() << endl;
        for (size_t j : rectIndices) {
            FloatRect innerMaxBounds = info.transformRect(boundsList[j]);
            if (!innerMaxBounds.intersects(rect)) continue;
            //Instead of checking intersects with rect, use maxbounds with rect to create a rect
            //whose intersects with this collider will provide the maximum location to translate to.
            FloatRect effectiveRect =rect;
            if (findExterior) {
                if (type == SurfaceType::GROUND || type == SurfaceType::CEILING) {
                    effectiveRect = FloatRect(rect.left, innerMaxBounds.top, rect.width, innerMaxBounds.height);
                }
                else {
                    effectiveRect = FloatRect(innerMaxBounds.left, rect.top, innerMaxBounds.width, rect.height);
                }
            }
            
            for (size_t i = (j > 0 ? boundIndices[j - 1] : 0); i <= boundIndices[j]; ++i) {
               
                Line edge = Line(info.transformPoint(points[i]), info.transformPoint(points[(i + 1) % points.size()]));
                unordered_set<Vector2f> interiorPoints = edge.findInteriorPoints(effectiveRect);
                unordered_set<Vector2f> intersections = edge.findAllIntersections(effectiveRect);
                
                intersections.insert(interiorPoints.begin(), interiorPoints.end());
                
                vector<Vector2f> foundPoints = { intersections.begin(), intersections.end() };
                //cout << foundPoints.size() << " ";
                //if (i == 3 && foundPoints.size() != 0) cout << edge.toString() << endl;
                for (const Vector2f& p : intersections) {
                    
                    if ((type == SurfaceType::GROUND && p.y > rect.top + rect.height) ||
                        (type == SurfaceType::CEILING && p.y < rect.top) ||
                        (type == SurfaceType::RIGHT && p.x > rect.left + rect.width) ||
                        (type == SurfaceType::LEFT && p.x < rect.left)) {
                        //if (i == 3 && type == SurfaceType::GROUND) cout << p.y << " " << rect.top + rect.height << endl;
                        isLegitimate = true;
                        continue;
                    }
                    else if (!foundIntersection ||
                        (type == SurfaceType::GROUND && p.y < bestIntersection.y) ||
                        (type == SurfaceType::CEILING && p.y > bestIntersection.y) ||
                        (type == SurfaceType::RIGHT && p.x < bestIntersection.x) ||
                        (type == SurfaceType::LEFT && p.x > bestIntersection.x)) {
                        //if (i == 3 && foundPoints.size() != 0) cout << "Maybe not" << endl;
                        bestIntersection = p;
                        foundIntersection = true;
                        //if (type == SurfaceType::LEFT && intersections.size() > 0) cout << p.x - rect.left << endl;
                        

                        //if (type == SurfaceType::LEFT) cout << toString(p) << " ";
                    }
                    if ((type == SurfaceType::GROUND && p.y >= rect.top) ||
                        (type == SurfaceType::CEILING && p.y <= rect.top + rect.height) ||
                        (type == SurfaceType::RIGHT && p.x >= rect.left) ||
                        (type == SurfaceType::LEFT && p.x <= rect.left + rect.width)) {
                        isLegitimate = true;
                    }
                }
               
            }
        }
        if (type == SurfaceType::GROUND || type == SurfaceType::CEILING) {
            //cout << toString(bestIntersection) << endl;
            return{ isLegitimate && foundIntersection, bestIntersection.y };
        }
        else return{isLegitimate && foundIntersection, bestIntersection.x };*/
    }
}

pair<bool, float> FreeformCollider::intersects(const sf::Transform& T, const Segment& line)const {
    Segment effectiveLine = Segment(T.getInverse().transformPoint(line.start()), T.getInverse().transformPoint(line.end()));
    pair<bool, float> intersection = quadtree.findIntersection(effectiveLine);
    //if (intersection.first) cout << intersection.second << ", " << toString(line.atPoint(intersection.second)) << endl;
    return intersection;
}

vector<Segment> FreeformCollider::intersects(const Polygon& shape, const sf::Transform& T) const {
    vector<Segment> segments =  colliderShape.findOverlappingLines(shape.transform(T.getInverse()));
    for (size_t i = 0; i < segments.size(); ++i) segments[i] = segments[i].transform(T);
    return segments;
}

bool FreeformCollider::contains(Vector2f& v, const Transform& localToWorld) const {
    return colliderShape.containsPoint(localToWorld.getInverse().transformPoint(v));
}
vector<Vector2f> FreeformCollider::findSurfacePoints(const Transform& info, const FloatRect& rect) const {
    FloatRect effectiveMaxBounds = info.transformRect(maxBounds);

    if (!effectiveMaxBounds.intersects(rect)) return{};
    else {
        vector<Segment> foundLines = {};
        vector<size_t> rectIndices = sorter.findIntersects(info.getInverse().transformRect(rect));

        for (size_t j  : rectIndices) {
            FloatRect innerMaxBounds = info.transformRect(boundsList[j]);
            if (!innerMaxBounds.intersects(rect)) continue;

            for (size_t i = (j > 0 ? boundIndices[j-1] : 0); i < boundIndices[j]; ++i) {
                Segment edge = Segment(info.transformPoint(points[i]), info.transformPoint(points[(i + 1) % points.size()]));

                pair<bool, Segment> innerLine = edge.findInnerLine(rect);
                if (innerLine.first) foundLines.push_back(innerLine.second);
                //cout << edge.toString() << endl;
            }
        }

        //Sort found lines by lines with the highest (minimum) start or end point.
        sort(foundLines.begin(), foundLines.end(), [](const Segment& l1, const Segment& l2) {
            float y1 = min(l1.start().y, l1.end().y);
            float y2 = min(l2.start().y, l2.start().y);
            return y1 > y2;
        });

        vector<FloatRect> rects = {};
        vector<Vector2f> output = {};
        for (const Segment& l : foundLines) {
            FloatRect lRect(min(l.start().x, l.end().x), min(l.start().y, l.end().x), 
                abs(l.start().x - l.end().x), abs(l.start().y - l.end().y));
            bool isLegitimate = true;
            for (const FloatRect& r : rects) {
                if (r.intersects(lRect)) {
                    isLegitimate = false;
                    break;
                }
            }
            if (isLegitimate) {
                output.push_back(l.start());
                output.push_back(l.end());
                //cout << l.toString() << endl;
            }
            rects.push_back(lRect);
        }
        //sort output according to smallest x coordinate to largest x coordinate.
        
        sort(output.begin(), output.end(), [](const Vector2f& v1, const Vector2f& v2) {
            return v1.x < v2.x;
        });
        return output;
    }
}

vector<Vector2f> FreeformCollider::findInteriorPoints(const sf::Transform& T, const sf::FloatRect& rect) const {
    FloatRect effectiveMaxBounds = T.transformRect(maxBounds);

    if (!effectiveMaxBounds.intersects(rect)) return{};
    else {
        vector<Vector2f> foundPoints = {};
        vector<size_t> rectIndices = sorter.findIntersects(T.getInverse().transformRect(rect));

        for (size_t j : rectIndices) {
            FloatRect innerMaxBounds = T.transformRect(boundsList[j]);
            if (!innerMaxBounds.intersects(rect)) continue;

            for (size_t i = (j > 0 ? boundIndices[j - 1] : 0); i < boundIndices[j]; ++i) {
                Segment edge = Segment(T.transformPoint(points[i]), T.transformPoint(points[(i + 1) % points.size()]));

                pair<bool, Segment> innerLine = edge.findInnerLine(rect);
                if (innerLine.first) {
                    foundPoints.push_back(innerLine.second.start());
                    foundPoints.push_back(innerLine.second.end());
                }
            }
        }
        return foundPoints;
    }
}


void FreeformCollider::draw(const Transform& info, RenderTarget& target, RenderStates states, const Color& color, bool debug) const {
    float ppu = GameInfo::pixelsPerUnit;
    VertexArray vertices = VertexArray(sf::LinesStrip, points.size());
    for (size_t i = 0; i < points.size(); ++i) {
        Vertex v = Vertex(info.transformPoint(points[i])*ppu, color);
        vertices[i] = v;
    }
    vertices.append(vertices[0]);
    target.draw(vertices, states);
    if (debug) {
        /*CustomUtilities::draw(maxBounds, Color::Red, target, states);
        for (const auto& bbox : boundsList) {
            CustomUtilities::draw(info.transformRect(bbox), Color::Green, target, states);
        }*/
        //quadtree.draw(target, states);
        colliderShape.drawTree(target, states);
    }
    
}