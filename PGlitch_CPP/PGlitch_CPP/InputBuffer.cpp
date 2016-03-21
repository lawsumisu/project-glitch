#include "stdafx.h"
#include "InputBuffer.h"
#include <iostream>

InputBuffer::InputBuffer(size_t size, InputBindingSet& bindings) : 
    inputs(RingBuffer<FrameInput>(size)) {
    this->size = size;
    this->bindings = {};
    this->bindings.insert(bindings.begin(), bindings.end());
    clear();
}

void InputBuffer::update() {
    FrameInput fi = FrameInput();
    FrameInput previousFI = current();
    for (const auto& binding : bindings) {
        bool inputCheck = binding->readInput();
        if (inputCheck) {
            //Check if previous frame input contained this binding as well, as that means this input is being held.
            if (previousFI.contains(PlayerInput(binding->code(), InputType::HOLD)) ||
                previousFI.contains(PlayerInput(binding->code(), InputType::PRESS))) {
                fi.add(PlayerInput(binding->code(), InputType::HOLD));
            }
            else fi.add(PlayerInput(binding->code(), InputType::PRESS));
        }
        else {
            //Check if previous frame input contained this binding, as it means this input has been released.
            if (previousFI.contains(PlayerInput(binding->code(), InputType::HOLD)) ||
                previousFI.contains(PlayerInput(binding->code(), InputType::PRESS))) {
                fi.add(PlayerInput(binding->code(), InputType::UP));
            }
        }
    }
    inputs.add(fi);
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

