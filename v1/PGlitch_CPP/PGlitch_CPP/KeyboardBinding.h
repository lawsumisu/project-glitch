#pragma once

#include <SFML/Window.hpp>
#include "InputBinding.h"

struct KeyboardBinding : public InputBinding {
private :
    InputCode code;
    sf::Keyboard::Key key;
    PlayerInput input;
public:
    KeyboardBinding(sf::Keyboard::Key key, InputCode code);
    PlayerInput readInput() const;
    bool operator==(const InputBinding* otherInput) const;
    bool operator==(const InputBinding& otherBinding) const;
    bool operator==(const KeyboardBinding otherBinding) const;
    size_t hashCode() const;
    std::string toString() const;

};