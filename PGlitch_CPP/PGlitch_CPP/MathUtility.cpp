#include "stdafx.h"
#include "MathUtility.h"

using namespace CustomUtilities;
using namespace sf;

int MathUtility::sign(float f) {
    if (f == 0) return 0;
    else return f == abs(f) ? 1 : -1; 
}
Vector2f MathUtility::linearRegression(std::vector<Vector2f> coordinates) {
    int count = coordinates.size();
    float x_sum = 0, y_sum = 0, xx_sum = 0, xy_sum = 0;
    for (Vector2f& v : coordinates) {
        x_sum += v.x;
        y_sum += v.y;
        xx_sum += v.x*v.x;
        xy_sum += v.x*v.y;
    }
    float determinant = count*xx_sum - x_sum*x_sum;
    //TODO make sure determinant != 0;
    float b = (xx_sum*y_sum - x_sum*xy_sum) / determinant;
    float m = (-x_sum*y_sum + count*xy_sum) / determinant;

    return Vector2f(b, m);
}