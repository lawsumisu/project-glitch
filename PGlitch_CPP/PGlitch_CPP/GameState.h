#pragma once

#include <SFML/System/Clock.hpp>
#include <iostream>
#include "GameTime.h"

using namespace sf;

class GameState {
private:
    //Fields
    size_t frameCount;
    GameTime _time;
    GameState();
public:
    void update();
    /// <summary>
    /// Resets the game state.
    /// </summary>
    void reset();
    /// <summary>
    /// Gets the <see cref="GameTime"/> of this <see cref="GameState"/>.
    /// </summary>
    /// <returns></returns>
    GameTime time() { return _time; }
    static GameState& GS() { 
        static GameState instance;
        return instance; 
    }
};