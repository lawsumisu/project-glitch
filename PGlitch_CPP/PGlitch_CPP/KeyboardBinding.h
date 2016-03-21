#pragma once

#include <SFML/Window.hpp>
#include "InputBinding.h"

struct KeyboardBinding : public InputBinding {
private :
    InputCode _code;
    sf::Keyboard::Key key;
public:
    KeyboardBinding(sf::Keyboard::Key key, InputCode code);
    bool readInput() const;
    InputCode code() const;
    bool operator==(const InputBinding* otherInput) const;
    bool operator==(const KeyboardBinding otherBinding) const;
    size_t hashCode() const;
    std::string toString() const;

};