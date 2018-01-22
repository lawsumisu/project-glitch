#include "stdafx.h"
#include "MathUtility.h"
#include <math.h>
#include <algorithm>
#include <iostream>
#include "VectorUtility.h"

using namespace CustomUtilities;
using namespace sf;
using namespace std;

int CustomUtilities::sign(float f) {
    if (f == 0) return 0;
    else return f == abs(f) ? 1 : -1; 
}
Vector2f CustomUtilities::linearRegression(std::vector<Vector2f> coordinates) {
    if (coordinates.size() <= 1) {
        cerr << "Error: Too few points: " << coordinates.size()  << endl;
        return Vector2f();

    }
    //In order to deal with overflow with large inputs, offset inputs and add them back at the end.
    float xMin = coordinates[0].x, yMin = coordinates[0].y;
    for (Vector2f& v : coordinates) {
        xMin = min(xMin, v.x);
        yMin = min(yMin, v.y);
    }
    int count = coordinates.size();
    double x_sum = 0, y_sum = 0, xx_sum = 0, xy_sum = 0;
    for (Vector2f& v : coordinates) {
        float vx = v.x - xMin;
        float vy = v.y - yMin;
        x_sum += vx;
        y_sum += vy;
        xx_sum += vx*vx;
        xy_sum += vx*vy;
    }
    double determinant = count*xx_sum - x_sum*x_sum;
    //TODO make sure determinant != 0;
    double b = (xx_sum*y_sum - x_sum*xy_sum) / determinant;
    double m = (-x_sum*y_sum + count*xy_sum) / determinant;

    //Due to potential rounding errors, round off m.
    double precision = 1000000000;
    m = round(m*precision) / precision;

    return Vector2f((float) (b + yMin - m*xMin), (float) m);
}

float CustomUtilities::degToRad(float degree) {
    return degree * pi / 180;
}

float CustomUtilities::clamp(float value, float f1, float f2) {
    float minValue = min(f1, f2);
    float maxValue = max(f1, f2);

    return min(max(value, minValue), maxValue);
}