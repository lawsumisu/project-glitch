#include "stdafx.h"
#include "FrameInput.h"

FrameInput::FrameInput(){
    inputs = {};
}

void FrameInput::add(PlayerInput& input) {
    if (input != PlayerInput::none()) {
        inputs.insert(input);
    }
}

bool FrameInput::contains(PlayerInput& input) const {
    if (input == PlayerInput::none()) return count() == 0;
    else return inputs.find(input) != inputs.end();
}

size_t FrameInput::count() const {
    return inputs.size();
}

std::string FrameInput::toString() const {
    if (count() == 0) return "<->";
    std::stringstream ss;
    ss << "<";
    bool addComma = false;
    for (const auto& input : inputs) {
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