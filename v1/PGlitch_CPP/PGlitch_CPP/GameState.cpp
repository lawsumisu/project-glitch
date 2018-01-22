#include "stdafx.h"
#include "GameState.h"

GameState::GameState(){}

void GameState::update() {
    ++GS().frameCount;
    GS()._time.update();
}

void GameState::reset() {
    GS()._time = GameTime();
}

GameTime& GameState::time() {
    return GS()._time;
}

