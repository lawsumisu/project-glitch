#pragma once

#include <SFML/System/Clock.hpp>
#include <iostream>
#include "GameTime.h"

using namespace sf;

class GameState {
private:
    //Fields

    size_t frameCount = 0;
    GameTime _time;

    //Methods
    /// <summary>
    /// Gets the singleton <see cref="GameState"/>.
    /// </summary>
    /// <returns></returns>
    static GameState& GS() {
        static GameState instance;
        return instance;
    }
    //Constructor

    GameState();
public:
    static void update();
    /// <summary>
    /// Resets the game state.
    /// </summary>
    static void reset();
    /// <summary>
    /// Gets the <see cref="GameTime"/> of this <see cref="GameState"/>.
    /// </summary>
    /// <returns></returns>
    static GameTime& time();
    
};