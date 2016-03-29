#pragma once
#include <string>

enum class InputType {NONE, PRESS, HOLD, RELEASE};
enum class InputCode {NONE, UP, DOWN, LEFT, RIGHT, B1, B2, B3, B4, B5, B6, B7, B8};

struct EnumeratorUtilities {
    static std::string toSymbol(const InputCode& code);
    static std::string toSymbol(const InputType& type);
};