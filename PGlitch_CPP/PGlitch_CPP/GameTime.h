#pragma once

#include <SFML/System/Clock.hpp>
#include "RingBuffer.h"
#include <iostream>

using namespace sf;
class GameState;
class GameTime {
private:
    Clock clock;
    float _timeScale;
    float cachedTime;
    RingBuffer<float> clockHistory;
    void update();
    
public:
    GameTime();
    float timeScale() const;
    void timeScale(float newTimeScale);
    /// <summary>
    /// Gets the delta time in seconds per frame.
    /// </summary>
    float dt() const;
    friend class GameState;
};
