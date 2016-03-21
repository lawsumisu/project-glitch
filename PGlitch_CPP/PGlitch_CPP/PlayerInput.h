#pragma once

#include "Enumerators.h"
#include <functional>

class PlayerInput {
private:
    InputCode _code;
    InputType _type;
    static PlayerInput _none;

public:
    PlayerInput(InputCode code, InputType type);
    /// <summary>
    /// Gets the <see cref="InputCode"/> of this <see cref="PlayerInput"/>.
    /// </summary>
    /// <returns></returns>
    InputCode code() const;

    /// <summary>
    /// Gets the <see cref="InputType"/> of this <see cref="PlayerInput"/>.
    /// </summary>
    /// <returns></returns>
    InputType type() const;

    bool operator==(const PlayerInput& otherInput) const;
    bool operator!=(const PlayerInput& otherInput) const;
    std::string toString() const;
    size_t hashCode() const;

    /// <summary>
    /// Gets a <see cref="PlayerInput"/> that refers to no input being made.
    /// </summary>
    static PlayerInput none();
};

namespace std {
    template <> struct std::hash<PlayerInput> {
        size_t operator()(const PlayerInput &input) const {
            return input.hashCode();
        }
    };
}
