#pragma once

#include "FrameInput.h"
#include "RingBuffer.h"
#include "InputBinding.h"
#include "KeyboardBinding.h"
#include "JoystickBinding.h"
#include <unordered_set>

typedef std::unordered_set<InputBindingPtr> InputBindingSet;

class InputBuffer{
private:
    RingBuffer<FrameInput> inputs;
    InputBindingSet bindings;
    size_t size;
public:
    /// <summary>
    /// Creates an <see cref="InputBuffer"/> of size size with no bindings.
    /// </summary>
    InputBuffer(size_t size);
    InputBuffer(size_t size, InputBindingSet& bindings);
    void addBinding(InputBinding* binding);
    void update();
    void clear();
    FrameInput current() const;
    FrameInput operator[](int i) const;
    size_t length() const;

};

