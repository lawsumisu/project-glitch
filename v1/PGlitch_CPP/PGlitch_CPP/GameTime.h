#pragma once

#include <SFML/System/Clock.hpp>
#include "RingBuffer.h"
#include <iostream>

using namespace sf;
class GameState;
class GameTime {
private:
    Clock clock;
    float _timeScale = 1.f;
    float _glitchedTimeScale = 1.f;
    int _frameCount = 0;
    float cachedTime;
    RingBuffer<float> clockHistory;
    void update();
    
public:
    GameTime();

    //Accessors

    float timeScale() const;
    void timeScale(float newTimeScale);

    /// <summary>
    /// Gets the glitched time scale.
    /// </summary>
    /// <returns></returns>
    float glitchedTimeScale() const;

    void glitchedTimeScale(float newGTS);

    /// <summary>
    /// Gets the delta time in seconds per frame.
    /// </summary>
    float dt() const;

    /// <summary>
    /// Gets the unscaled delta time in seconds per frame.
    /// </summary>
    /// <returns></returns>
    float udt() const;

    /// <summary>
    /// Gets the glitched delta time in seconds per frame.
    /// </summary>
    float gdt() const;
    friend class GameState;
    int frameCount() const;
    float timestamp() const;
};
