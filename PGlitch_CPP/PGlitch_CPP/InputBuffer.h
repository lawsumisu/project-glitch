#pragma once

#include "FrameInput.h"
#include "RingBuffer.h"
#include "InputBinding.h"
#include "KeyboardBinding.h"
#include <unordered_set>

typedef std::unordered_set<InputBindingPtr> InputBindingSet;

class InputBuffer{
private:
    RingBuffer<FrameInput> inputs;
    InputBindingSet bindings;
    size_t size;
public:
    InputBuffer(size_t size, InputBindingSet& bindings);
    void update();
    void clear();
    FrameInput current() const;
    FrameInput operator[](int i) const;
    size_t length() const;

};

