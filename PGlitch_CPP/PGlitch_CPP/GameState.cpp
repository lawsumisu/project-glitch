#include "stdafx.h"
#include "GameState.h"

GameState::GameState(){}

void GameState::update() {
    ++frameCount;
    _time.update();
}

void GameState::reset() {
    _time = GameTime();
}

