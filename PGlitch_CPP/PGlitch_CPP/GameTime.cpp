#include "stdafx.h"
#include "GameTime.h"

GameTime::GameTime() : clockHistory(RingBuffer<float>(500)) {
    clockHistory.add(0);
    cachedTime = 0;
}

void GameTime::update() {
    clockHistory.add(clock.getElapsedTime().asSeconds());
    cachedTime = clockHistory[-1] - clockHistory[0];
    ++_frameCount;
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
int GameTime::frameCount() const {
    return _frameCount;
}

float GameTime::timestamp() const {
    return clock.getElapsedTime().asSeconds();
}