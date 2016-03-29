#include "stdafx.h"
#include "KeyboardBinding.h"


KeyboardBinding::KeyboardBinding(sf::Keyboard::Key key, InputCode code) {
    this->key = key;
    _code = code;
}

InputCode KeyboardBinding::code() const {
    return _code;
}

bool KeyboardBinding::readInput() const {
    return sf::Keyboard::isKeyPressed(key);
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
    return key == otherBinding.key && _code == otherBinding._code;
}
size_t KeyboardBinding::hashCode() const {
    return std::hash<int>()((int)_code) ^ std::hash<int>()((int)key);
}

std::string KeyboardBinding::toString() const {
    std::stringstream ss;
    ss << "[KeyboardBinding: binding = " << EnumeratorUtilities::toSymbol(_code) << "]";
    return ss.str();
}