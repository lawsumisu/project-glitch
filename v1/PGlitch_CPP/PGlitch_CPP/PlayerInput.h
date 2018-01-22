#pragma once

#include "Enumerators.h"
#include <functional>

class PlayerInput {
private:
    //Fields
    InputCode _code;
    int _duration;
    float _strength;
    InputType _type;

public:
    //Constructors
    PlayerInput();
    PlayerInput(InputCode code, InputType type, float strength = 100.f);
    PlayerInput(InputCode code, int duration = 1, float strength = 100.f);

    //Methods
    /// <summary>
    /// Gets the <see cref="InputCode"/> of this <see cref="PlayerInput"/>.
    /// </summary>
    /// <returns></returns>
    InputCode code() const { return _code; }

    /// <summary>
    /// Gets the number of frames this input has been held. It is 1 for inputs executed this frame.
    /// A negative duration corresponds to how long the input has been released.
    /// </summary>
    /// <returns></returns>
    int duration() const { return _duration; }

    /// <summary>
    /// Gets the strength of this input, which is a nonnegative value in the range of [0,100].
    /// </summary>
    /// <returns></returns>
    float strength() const { return _strength; }
 
    /// <summary>
    /// Creates a new <see cref="PlayerInput"/> with the same properties as this one,
    /// except for a new <see cref="InputType"/>. 
    /// </summary>
    /// <param name="newType">new type to change to. Cannot be InputType::NONE.</param>
    PlayerInput retype(InputType newType) const;

    /// <summary>
    /// Creates a new <see cref="PlayerInput"/> with the same properties as this one,
    /// except for a new strength. 
    /// </summary>
    /// <param name="newStrength">new strength to tune to. Must be [0,100].</param>
    PlayerInput retune(float newStrength) const;

    bool operator==(const PlayerInput& otherInput) const;
    bool operator!=(const PlayerInput& otherInput) const;
    std::string toString() const;
    size_t hashCode() const;

    /// <summary>
    /// Gets a <see cref="PlayerInput"/> that refers to no input being made.
    /// </summary>
    static PlayerInput& none();
};

namespace std {
    template <> struct std::hash<PlayerInput> {
        size_t operator()(const PlayerInput &input) const {
            return input.hashCode();
        }
    };
}
