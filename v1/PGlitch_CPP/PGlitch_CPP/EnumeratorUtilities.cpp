#include "stdafx.h"
#include "Enumerators.h"

using namespace std;

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
    case InputCode::B1:
        return "B1";
    case InputCode::B2:
        return "B2";
    case InputCode::B3:
        return "B3";
    case InputCode::B4:
        return "B4";
    case InputCode::B5:
        return "B5";
    case InputCode::B6:
        return "B6";
    case InputCode::B7:
        return "B7";
    case InputCode::B8:
        return "B8";

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

std::string toSymbol(PlatformType type) {
    switch (type) {
    case PlatformType::SOLID: return "SOLID";
    case PlatformType::THICK: return "THICK";
    case PlatformType::THIN: return "THIN";
    default: return "unidentified platform type";
    }
}

ostream& operator<<(ostream& os, PlatformType& type) {
    os << toSymbol(type);
    return os;
}