#pragma once

#include <unordered_map>
#include "PlayerInput.h"
#include <sstream>

/// <summary>
/// Represents a series of <see cref="PlayerInput"/>s inputted in a single frame.
/// </summary>
class FrameInput {
private:
    std::unordered_map<InputCode, float> codeToStrengthMap;
    std::unordered_map<InputCode, int> codeToDurationMap;

public:
    FrameInput();
    /// <summary>
    /// Adds a <see cref="PlayerInput"/> to this <see cref="FrameInput"/>. Inputs matching "none" are not
    /// added. 
    /// Strengths and durations have an ordering; inputs of higher strength and duration 
    /// are stored over inputs of lower strength and duration.
    /// In this way, inputs can be partially added; for example, if L(10) was previously added, adding [+]L(5)
    /// results in [+]L(10) being stored.   
    /// </summary>
    /// <param name="input"></param>
    void add(const PlayerInput& input);
    bool contains(const PlayerInput& input) const;
    bool containsIgnoreStrength(const PlayerInput& input) const;
    /// <summary>
    /// Checks if this <see cref="FrameInput"/> contains a <see cref="PlayerInput"/> that represents
    /// the <see cref="InputCode"/> that is either pressed or held.
    /// </summary>
    bool isDown(InputCode code) const;
    /// <summary>
    /// Returns a <see cref="FrameInput"/> that represents inputs executed this frame with respect to
    /// inputs executed in a previous frame. Useful to get held and released inputs.
    /// </summary>
    /// <param name="previous"></param>
    /// <returns></returns>
    FrameInput modifyWithPrevious(FrameInput& previous) const;
    /// <summary>
    /// Gets the <see cref="PlayerInput"/> that corresponds to the specified <see cref="InputCode"/>.
    /// Requires that code not be NONE. If this <see cref="FrameInput"/> doesn't contain this code, then returns
    /// a "none" input.
    /// </summary>
    PlayerInput operator[](InputCode code) const;
    std::size_t count() const;   
    std::string toString() const;
    friend std::ostream& operator<<(std::ostream& s, const FrameInput& fi);
};

