#pragma once

#include <unordered_set>
#include "PlayerInput.h"
#include <sstream>

class FrameInput {
private:
    std::unordered_set<PlayerInput> inputs;
public:
    FrameInput();
    void add(PlayerInput& input);
    bool contains(PlayerInput& input) const;
    /// <summary>
    /// Checks if this <see cref="FrameInput"/> contains a <see cref="PlayerInput"/> that represents
    /// the <see cref="InputCode"/> that is either pressed or held.
    /// </summary>
    bool isDown(InputCode code) const;
    std::size_t count() const;   
    std::string toString() const;
    friend std::ostream& operator<<(std::ostream& s, const FrameInput& fi);
};

