#include "stdafx.h"
#include "GameTime.h"
#include <iostream>

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
    return udt() * _timeScale;
}

float GameTime::udt() const {
    return cachedTime / (clockHistory.length() - 1);
}

float GameTime::gdt() const {
    return dt() * _glitchedTimeScale;
}

void GameTime::timeScale(float newTimeScale) {
    _timeScale = newTimeScale;
}

float GameTime::timeScale() const {
    return _timeScale;
}

void GameTime::glitchedTimeScale(float newGTS) {
    _glitchedTimeScale = newGTS;
}

float GameTime::glitchedTimeScale() const {
    return _glitchedTimeScale;
}
int GameTime::frameCount() const {
    return _frameCount;
}

float GameTime::timestamp() const {
    return clock.getElapsedTime().asSeconds();
}