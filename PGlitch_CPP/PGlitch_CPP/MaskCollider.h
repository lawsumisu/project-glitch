#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System.hpp>
#include <vector>

namespace Physics {
    
    struct Collision {

    };
    class MaskCollider {
    private:
        std::vector<float> heightMap;
        float width;
        Vector2f position;
      
    public:
        MaskCollider(std::vector<float> heightMap, float width);

    };
}