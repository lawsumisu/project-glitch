#pragma once

#include "InputBuffer.h"
/// <summary>
/// Object that represents a playable character.
/// </summary>
class Character {

private:
    InputBuffer buffer;
public:
    void update();

};