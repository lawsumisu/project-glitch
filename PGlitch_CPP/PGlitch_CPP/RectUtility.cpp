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


RectSorter::RectSorter(const vector<FloatRect>& rects): xEndToStartLinks(rects.size()), xStartToEndLinks(rects.size()) {
    this->rects = rects;
    size_t n = rects.size();
    for (size_t i = 0; i < rects.size(); ++i) {
        RValue sx(i, rects[i].left, true);
        RValue ex(i, rects[i].left + rects[i].width, false);
        RValue sy(i, rects[i].top, true);
        RValue ey(i, rects[i].top + rects[i].height, false);

        xIndices.push_back(sx);
        xIndices.push_back(ex);
        yIndices.push_back(sy);
        yIndices.push_back(ey);

        xStartIndices.push_back(i);
        xEndIndices.push_back(i);
        yStartIndices.push_back(i);
        yEndIndices.push_back(i);
    }

    //Sort the index lists.
    sort(xIndices.begin(), xIndices.end());
    sort(yIndices.begin(), yIndices.end());

    sort(xStartIndices.begin(), xStartIndices.end(), [rects](size_t i, size_t j) {
        return rects[i].left < rects[j].left;
    });
    sort(xEndIndices.begin(), xEndIndices.end(), [rects](size_t i, size_t j) {
        return rects[i].left+ rects[i].width < rects[j].left + rects[j].width;
    });
    sort(yStartIndices.begin(), yStartIndices.end(), [rects](size_t i, size_t j) {
        return rects[i].top < rects[j].top;
    });
    sort(yEndIndices.begin(), yEndIndices.end(), [rects](size_t i, size_t j) {
        return rects[i].top + rects[i].height < rects[j].top + rects[j].height;
    });

    cout << toString() << endl;

    //Now build links between start and end indices.

    unordered_map<size_t, size_t> xEndMap;
    for (size_t i = 0; i < xEndIndices.size(); ++i) {
        xEndMap.insert({ xEndIndices[i], i });
        xStartToEndLinks.push_back(0);
    }
    for (size_t i = 0; i < n; ++i) {
        xStartToEndLinks[i] = xEndMap[xStartIndices[i]];
        xEndToStartLinks[xEndMap[xStartIndices[i]]] = i;
    }
}

int RectSorter::find(float value, size_t start, size_t end, bool xNotY, bool startNotEnd) const {
    float sv = rvalue(start, xNotY, !startNotEnd);
    float ev = rvalue(end, xNotY, !startNotEnd);

    //Base cases
    //BO: start and end are same,so do simple check for intersection.
    if (start == end) {
        if ((startNotEnd && value < ev) || (!startNotEnd && value > sv)) return end;
        else return -1;
    }
    else if (startNotEnd && value < sv) return start;
    else if (!startNotEnd && value > ev) return end;
    //B1: fail if looking for a start beyond the end indices or if looking for a end before the start indices.
    else if ((startNotEnd && value >= ev) || (!startNotEnd && value <= sv)) return -1;
    else if (end - start == 1) {
        //B2: if searching between 2 indices, return either start or end as appropriate.
        if (startNotEnd) {
            if (value < sv) return start;
            else return end;
        }
        else {
            if (value > ev) return end;
            else return start;
        }
    }
    else {
        //Recursive case:
        size_t i = (start + end) / 2;
        if (startNotEnd) {
            if (value < rvalue(i, xNotY, !startNotEnd)) return (find(value, start, i, xNotY, startNotEnd));
            else return find(value, i, end, xNotY, startNotEnd);
        }
        else {
            if (value > rvalue(i, xNotY, !startNotEnd))return find(value, i,end, xNotY, startNotEnd);
            else return find(value, start, i, xNotY, startNotEnd);
        }
        
    }
}

float RectSorter::rvalue(size_t i, bool xNotY, bool startNotEnd) const {
    if (xNotY) {
        if (startNotEnd) {
            return rects[xStartIndices[i]].left;
        }
        else return rects[xEndIndices[i]].left + rects[xEndIndices[i]].width;
    }
    else {
        if (startNotEnd) return rects[yStartIndices[i]].top;
        else return rects[yEndIndices[i]].top + rects[yEndIndices[i]].height;
    }
}

vector<size_t> RectSorter::findIntersects(const FloatRect& rect) const {
    int xMinEnd = find(rect.left, 0, xStartIndices.size()-1, true, true);
    int xMaxStart = find(rect.left + rect.width, 0, xEndIndices.size() - 1, true, false);
    if (xMinEnd == -1 || xMaxStart == -1) return{};
    else {
        set<size_t> xIndices = {};
        //Choose the smaller distance from which to build output.
        if (rects.size() - (size_t) xMinEnd -1 < (size_t) xMaxStart) {
            //cout << rects.size() - (size_t)xMinEnd << endl;
            size_t upperStartIndex = xEndToStartLinks[xMinEnd];
            for (size_t i = xMinEnd; i < rects.size(); ++i) {
                if (xEndToStartLinks[i] <= (size_t) xMaxStart) xIndices.insert(xEndIndices[i]);
            }
        }
        else {
           // cout << "Or " << xMaxStart << endl;
            for (size_t i = 0; i <= (size_t)xMaxStart; ++i) {
                if (xStartToEndLinks[i] >= (size_t) xMinEnd) xIndices.insert(xStartIndices[i]);
            }
        }

        return{xIndices.begin(),xIndices.end()};
    }
}

string RectSorter::toString() const {
    stringstream ss;
    ss << "RectSorter: [<";
    for (size_t i = 0; i < xStartIndices.size(); ++i) {
        ss << "S" << "x" << xStartIndices[i] << (i == xStartIndices.size()-1? "" : ", ");
    }
    ss << ">; <";
    for (size_t i = 0; i < xEndIndices.size(); ++i) {
        ss << "Ex" << xEndIndices[i] << (i == xEndIndices.size() - 1 ? "" : ", ");
    }
    ss << ">]";
    return ss.str();
}
