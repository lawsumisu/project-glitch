#include "stdafx.h"
#include "Enumerators.h"

std::string EnumeratorUtilities::toSymbol(const InputCode& code) {
    switch (code) {
    case InputCode::NONE:
        return "-";
    case InputCode::UP:
        return "U";
    case InputCode::DOWN:
        return "D";
    case InputCode::LEFT:
        return "L";
    case InputCode::RIGHT:
        return "R";
    default:
        return "";
    }
}

std::string EnumeratorUtilities::toSymbol(const InputType& type) {
    switch (type) {
    case InputType::HOLD:
        return "_";
    case InputType::RELEASE:
        return "^";
    default:
        return "";
    }
}