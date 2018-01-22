#pragma once
#include <string>
#include <iostream>

enum class InputType {NONE, PRESS, HOLD, RELEASE};
enum class InputCode {NONE, UP, DOWN, LEFT, RIGHT, B1, B2, B3, B4, B5, B6, B7, B8};
enum class PlatformType {SOLID, THIN, THICK};
enum class SurfaceType {GROUND, CEILING, RIGHT, LEFT};

struct EnumeratorUtilities {
    static std::string toSymbol(const InputCode& code);
    static std::string toSymbol(const InputType& type);
};

std::string toSymbol(PlatformType type);
std::ostream& operator<<(std::ostream& os, PlatformType& type);
