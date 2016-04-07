#include "stdafx.h"
#include "GameTime.h"

GameTime::GameTime() : clockHistory(RingBuffer<float>(200)) {
    clockHistory.add(0);
    _timeScale = 1;
    cachedTime = 0;
}

void GameTime::update() {
    clockHistory.add(clock.getElapsedTime().asSeconds());
    cachedTime = clockHistory[-1] - clockHistory[0];
}

float GameTime::dt() const {
    return cachedTime * _timeScale / (clockHistory.length() - 1);
}

void GameTime::timeScale(float newTimeScale) {
    _timeScale = newTimeScale;
}

float GameTime::timeScale() const {
    return _timeScale;
}