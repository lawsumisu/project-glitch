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
    std::size_t count() const;   
    std::string toString() const;
    friend std::ostream& operator<<(std::ostream& s, const FrameInput& fi);
};

