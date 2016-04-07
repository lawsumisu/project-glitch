#include "stdafx.h"
#include "FrameInput.h"

FrameInput::FrameInput(){
    codeToStrengthMap = {};
}

void FrameInput::add(const PlayerInput& input) {
    if (input != PlayerInput::none()) {
        //Add this input to the map, only if new or higher priority input.
        if (codeToStrengthMap.find(input.code()) == codeToStrengthMap.end()) {
            codeToStrengthMap[input.code()] = input.strength();
            codeToDurationMap[input.code()] = input.duration();
        }

        if (codeToStrengthMap.find(input.code())->second < input.strength()) {
            codeToStrengthMap[input.code()] = input.strength();
        }
        if (codeToDurationMap.find(input.code())->second < input.duration()) {
            codeToDurationMap[input.code()] = input.duration();
        }
    }
}

bool FrameInput::contains(const PlayerInput& input) const {
    if (containsIgnoreStrength(input)) {
        if (input == PlayerInput::none()) return true;
        return codeToStrengthMap.find(input.code())->second == input.strength();
    }
    return false;
}

bool FrameInput::containsIgnoreStrength(const PlayerInput& input) const {
    if (input == PlayerInput::none()) return count() == 0;
    else return codeToDurationMap.find(input.code()) != codeToDurationMap.end() && 
        codeToDurationMap.find(input.code())->second == input.duration();
}

bool FrameInput::isDown(InputCode code) const {
    if (code == InputCode::NONE) return false;
    else return codeToDurationMap.find(code)->second >= 1;
}
size_t FrameInput::count() const {
    return codeToStrengthMap.size();
}

FrameInput FrameInput::modifyWithPrevious(FrameInput& previous) const {
    FrameInput output = FrameInput();
    for (const auto& keyValuePair : codeToDurationMap) {
        //Check for held inputs.
        PlayerInput input = PlayerInput(keyValuePair.first, keyValuePair.second, 
            codeToStrengthMap.find(keyValuePair.first)->second);
        if (previous.isDown(keyValuePair.first)) {
            output.add(previous[keyValuePair.first].retype(InputType::HOLD).retune(input.strength()));
        }      
        else output.add(input);
    }

    //Now check for released inputs.
    for (const auto& keyValuePair : previous.codeToDurationMap) {
        if (previous.isDown(keyValuePair.first) && !isDown(keyValuePair.first)) {
            output.add(PlayerInput(keyValuePair.first, -1, 0.f));
        }
    }
    return output;
}

PlayerInput FrameInput::operator[] (InputCode code) const {
    if (codeToDurationMap.find(code) != codeToDurationMap.end()) {
        return PlayerInput(code, codeToDurationMap.find(code)->second, codeToStrengthMap.find(code)->second);
    }
    else return PlayerInput::none();
}
std::string FrameInput::toString() const {
    if (count() == 0) return "<->";
    std::stringstream ss;
    ss << "<";
    bool addComma = false;
    for (const auto& keyValuePair : codeToStrengthMap) {
        PlayerInput input = PlayerInput(keyValuePair.first, codeToDurationMap.find(keyValuePair.first)->second, 
            keyValuePair.second);
        if (addComma) ss << ",";
        ss << input.toString();
        addComma = true;
    }
    ss << ">";
    return ss.str();
}

std::ostream& operator<<(std::ostream& s, FrameInput const& fi) {
    return s << fi.toString();
}