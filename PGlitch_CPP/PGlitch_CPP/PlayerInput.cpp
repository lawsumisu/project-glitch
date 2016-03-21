#include "stdafx.h"
#include "PlayerInput.h"


PlayerInput PlayerInput::_none = PlayerInput(InputCode::NONE, InputType::NONE);

PlayerInput::PlayerInput(InputCode code, InputType type) {
    _code = code;
    _type = type;
}

InputCode PlayerInput::code() const {
    return _code;
}

InputType PlayerInput::type() const {
    return _type;
}

bool PlayerInput::operator==(const PlayerInput& otherInput) const {
    return _code == otherInput._code && _type == otherInput._type;
}

bool PlayerInput::operator!=(const PlayerInput& otherInput) const {
    return !((*this) == otherInput);
}

std::string PlayerInput::toString() const {
    return EnumeratorUtilities::toSymbol(_type) + EnumeratorUtilities::toSymbol(_code);
}

size_t PlayerInput::hashCode() const {
    return std::hash<int>()((int)_code) ^ std::hash<int>()((int)_type);
}

PlayerInput PlayerInput::none() {
    return _none;
}
