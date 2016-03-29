#pragma once

#include "PlayerInput.h"
#include <memory>
#include <sstream>

struct InputBinding {

    /// <summary>
    /// Gets the code that is bound to this input.
    /// </summary>
    virtual InputCode code() const = 0;
    /// <summary>
    /// Gets the state of the input and returns a <see cref="PlayerInput"/> that corresponds to its state. 
    /// </summary>
    virtual bool readInput() const = 0;

    virtual bool operator==(const InputBinding* otherIB) const = 0;

    virtual bool operator==(const InputBinding& otherIB) const = 0;

    virtual size_t hashCode() const = 0;

    inline bool operator!=(const InputBinding* otherIB) const {
        return !((*this) == otherIB);
    }

    virtual std::string toString() const = 0;
};

namespace std {
    template <> struct std::hash<std::shared_ptr<InputBinding>> {
        size_t operator()(const shared_ptr<InputBinding> &input) const {
            return input->hashCode();
        }
    };

    template <> struct std::equal_to<std::shared_ptr<InputBinding>> {
        bool operator()(const shared_ptr<InputBinding>& input1, const shared_ptr<InputBinding>& input2) const {
            return (*input1) == (input2.get());
            
        }
    };

    template <> struct std::hash<InputBinding> {
        size_t operator()(const InputBinding& input) const {
            return input.hashCode();
        }
    };

    template <> struct std::equal_to<InputBinding> {
        bool operator()(const InputBinding& input1, const InputBinding& input2) const {
            return input1 == input2;
        }
    };
}

typedef std::shared_ptr<InputBinding> InputBindingPtr;