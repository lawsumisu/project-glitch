#include "stdafx.h"
#include "KeyboardBinding.h"


KeyboardBinding::KeyboardBinding(sf::Keyboard::Key key, InputCode code) {
    this->key = key;
    this->code = code;
    input = PlayerInput(code, InputType::PRESS);

}


PlayerInput KeyboardBinding::readInput() const {
    if (sf::Keyboard::isKeyPressed(key)) return input;
    return PlayerInput::none();
}

bool KeyboardBinding::operator==(const InputBinding* otherInput) const {
    const KeyboardBinding* KB = dynamic_cast<const KeyboardBinding*> (otherInput);
    if (KB) return  (*this) == (*KB);
    else return false;
}

bool KeyboardBinding::operator==(const InputBinding& otherBinding) const {
    const KeyboardBinding* KB = dynamic_cast<const KeyboardBinding*>(&otherBinding);
    if (KB) return (*this) == (*KB);
    else return false;
}
bool KeyboardBinding::operator==(const KeyboardBinding otherBinding) const {
    return key == otherBinding.key && code == otherBinding.code;
}
size_t KeyboardBinding::hashCode() const {
    return std::hash<int>()((int)code) ^ std::hash<int>()((int)key);
}

std::string KeyboardBinding::toString() const {
    std::stringstream ss;
    ss << "[KeyboardBinding: binding = " << EnumeratorUtilities::toSymbol(code) << "]";
    return ss.str();
}