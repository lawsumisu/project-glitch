#include "stdafx.h"
#include "InputBuffer.h"
#include <iostream>

InputBuffer::InputBuffer(size_t size) : InputBuffer(size, InputBindingSet()) {}
InputBuffer::InputBuffer(size_t size, InputBindingSet& bindings) : 
    inputs(RingBuffer<FrameInput>(size)) {
    this->size = size;
    this->bindings = {};
    this->bindings.insert(bindings.begin(), bindings.end());
    clear();
}
void InputBuffer::addBinding(InputBinding* binding) {
    bindings.insert(InputBindingPtr(binding));
}
void InputBuffer::update() {
    FrameInput fi = FrameInput();
    FrameInput previousFI = current();
    for (const auto& binding : bindings) {
        PlayerInput input = binding->readInput();
        fi.add(binding->readInput());
    }
    inputs.add(fi.modifyWithPrevious(current()));
}

void InputBuffer::clear() {
    inputs = RingBuffer<FrameInput>(size);
    for (size_t i = 0; i < size; ++i) {
        inputs.add(FrameInput());
    }
}

FrameInput InputBuffer::current() const {
    return inputs[-1];
}

FrameInput InputBuffer::operator[](int i) const {
    return inputs[i];
}

size_t InputBuffer::length() const {
    return size;
}

