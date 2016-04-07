#include "stdafx.h"
#include "PlayerInput.h"
#include <sstream>
#include <algorithm>

//Constructors
PlayerInput::PlayerInput(InputCode code, int duration, float strength) : 
    _code(code), _duration(duration), _strength(strength) {
    if (duration == 1) _type = InputType::PRESS;
    else if (duration > 1) _type = InputType::HOLD;
    else if (duration < 0) _type = InputType::RELEASE;
    else _type = InputType::NONE;
}

PlayerInput::PlayerInput(InputCode code, InputType type, float strength) : 
    _code(code), _strength(strength), _type(type){
    switch (type) {
    case(InputType::PRESS) : _duration = 1; break;
    case(InputType::HOLD) : _duration = 2; break;
    case(InputType::RELEASE) : _duration = -1; break;
    default: _duration = 0; break;
    }
}

PlayerInput::PlayerInput() : PlayerInput(InputCode::NONE, 0, 0.f) {}
PlayerInput PlayerInput::retype(InputType newType) const {
    switch (newType) {
    case(InputType::HOLD) :
        return PlayerInput(_code, std::max(2, _duration + 1), _strength);
    case(InputType::PRESS) :
        return PlayerInput(_code, 1, _strength);
    case(InputType::RELEASE) :
        return PlayerInput(_code, std::min(-1, _duration - 1), _strength);
    default:
        return PlayerInput::none();
    }
}

PlayerInput PlayerInput::retune(float newStrength) const {
    return PlayerInput(_code, _duration, newStrength);
}

bool PlayerInput::operator==(const PlayerInput& otherInput) const {
    return _code == otherInput._code && _duration == otherInput._duration && _strength == otherInput._strength;
}

bool PlayerInput::operator!=(const PlayerInput& otherInput) const {
    return !((*this) == otherInput);
}

std::string PlayerInput::toString() const {
    int effectiveDuration = _duration;
    if (_duration >= 1) --effectiveDuration;
    std::stringstream ss;
    if (effectiveDuration != 0) {
        ss << "[" << (effectiveDuration > 0 ? "+" : "-");
        if (std::abs(effectiveDuration) > 1) ss << std::abs(effectiveDuration);
        ss << "]";
    }
    ss << EnumeratorUtilities::toSymbol(_code) << "(" << _strength << ")";
    return ss.str();
}

size_t PlayerInput::hashCode() const {
    return std::hash<int>()((int)_code) ^ std::hash<int>()(_duration) ^ std::hash<float>()(_strength);
}

PlayerInput& PlayerInput::none() {
    static PlayerInput instance;
    return instance;
}
