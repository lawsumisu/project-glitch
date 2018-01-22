#include "stdafx.h"
#include "JoystickBinding.h"
#include "MathUtility.h"

using namespace sf;
using namespace CustomUtilities;

// ============= //
// ButtonBinding //
// ============= //

ButtonBinding::ButtonBinding(size_t index, size_t button, InputCode code) {
    this->code = code;
    this->index = index;
    this->button = button;
    input = PlayerInput(code, InputType::PRESS);

}

PlayerInput ButtonBinding::readInput() const {
    if (Joystick::isButtonPressed(index, button)) return input;
    return PlayerInput::none();
}

bool ButtonBinding::operator==(const InputBinding* otherIB) const {
    const ButtonBinding* BB = dynamic_cast<const ButtonBinding*> (otherIB);
    if (BB) return  (*this) == (*BB);
    else return false;
}

bool ButtonBinding::operator==(const InputBinding& otherIB) const {
    return (*this) == dynamic_cast<const InputBinding*>(&otherIB);
}

bool ButtonBinding::operator==(const ButtonBinding otherBB) const {
    return index == otherBB.index && code == otherBB.code && button == otherBB.button;
}


size_t ButtonBinding::hashCode() const {
    return std::hash<int>()((int)code) ^ std::hash<int>()(button) ^ std::hash<int>()(index);
}

std::string ButtonBinding::toString() const {
    std::stringstream ss;
    ss << "[JoystickBinding(Button): binding = " << EnumeratorUtilities::toSymbol(code) << "]";
    return ss.str();
}

// =========== //
// AxisBinding //
// =========== //

AxisBinding::AxisBinding(size_t index, Joystick::Axis axis, InputCode positiveCode, InputCode negativeCode, float deadzone) {
    this->index = index;
    this->positiveCode = positiveCode;
    this->negativeCode = negativeCode;
    this->axis = axis;
    this->deadzone = deadzone;
}

PlayerInput AxisBinding::readInput() const {
    float read = Joystick::getAxisPosition(index, axis);
    if (abs(read) > deadzone) {
        return read > 0 ? 
            PlayerInput(positiveCode, InputType::PRESS, read) : PlayerInput(negativeCode, InputType::PRESS, -read);
    }
    return PlayerInput::none();
   
}

bool AxisBinding::operator==(const InputBinding* otherIB) const {
    const AxisBinding* AB = dynamic_cast<const AxisBinding*> (otherIB);
    if (AB) return  (*this) == (*AB);
    else return false;
}

bool AxisBinding::operator==(const InputBinding& otherIB) const {
    return (*this) == dynamic_cast<const InputBinding*>(&otherIB);
}

bool AxisBinding::operator==(const AxisBinding otherAB) const {
    return index == otherAB.index && positiveCode == otherAB.positiveCode && negativeCode == otherAB.negativeCode;
}


size_t AxisBinding::hashCode() const {
    return std::hash<int>()((int)positiveCode) ^ std::hash<int>()((int)axis) ^ std::hash<int>()((int)negativeCode)
        ^ std::hash<int>()(index);
}

std::string AxisBinding::toString() const {
    std::stringstream ss;
    ss << "[JoystickBinding(Axis): binding = +(" << EnumeratorUtilities::toSymbol(positiveCode) << "), -(" <<
        EnumeratorUtilities::toSymbol(negativeCode) << ")]";
    return ss.str();
}
