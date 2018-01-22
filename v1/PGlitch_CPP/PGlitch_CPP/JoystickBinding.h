#pragma once

#include "InputBinding.h"
#include <SFML/Window/Joystick.hpp>

/// <summary>
/// Object for representing the binding between a physical joystick input and it's abstract interpretation.
/// Immutable.
/// </summary>
class JoystickBinding : public InputBinding {
protected:
    //Fields
    size_t index;

public:
    //Methods
    virtual PlayerInput readInput() const = 0;
    virtual bool operator==(const InputBinding* otherIB) const= 0;
    virtual bool operator==(const InputBinding& otherIB) const= 0;
    virtual size_t hashCode() const = 0;
    virtual std::string toString() const= 0;
};

class ButtonBinding : public JoystickBinding {
private:
    InputCode code;
    size_t button;
    PlayerInput input;
public:
    //Constructor
    ButtonBinding(size_t index, size_t button, InputCode code);

    //Methods
    PlayerInput readInput() const;
    bool operator==(const InputBinding* otherIB) const;
    bool operator==(const InputBinding& otherIB) const;
    bool operator==(const ButtonBinding otherBB) const;
    size_t hashCode() const;
    std::string toString() const;
};

class AxisBinding : public JoystickBinding {
private:
    InputCode positiveCode, negativeCode;
    sf::Joystick::Axis axis;
    float deadzone;
public:

    //Constructor
    AxisBinding(size_t index, sf::Joystick::Axis axis, InputCode positiveCode, InputCode negativeCode, float deadzone = 25.f);
    //Methods
    PlayerInput readInput() const;
    bool operator==(const InputBinding* otherIB) const;
    bool operator==(const InputBinding& otherIB) const;
    bool operator==(const AxisBinding otherAB) const;
    size_t hashCode() const;
    std::string toString() const;
};
