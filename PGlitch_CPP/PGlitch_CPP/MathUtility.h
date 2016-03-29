#pragma once

#include <math.h>
namespace CustomUtilities {
    struct MathUtility {
        static int sign(float f) { return f == abs(f) ? 1 : -1; }
    };
}