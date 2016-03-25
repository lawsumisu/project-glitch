#include "stdafx.h"
#include "RectUtility.h"

using namespace CustomUtilities;

FloatRect RectUtility::construct(Vector2f center, Vector2f dimensions) {
    return FloatRect(center.x - dimensions.x / 2, center.y - dimensions.y / 2, dimensions.x, dimensions.y);
}
