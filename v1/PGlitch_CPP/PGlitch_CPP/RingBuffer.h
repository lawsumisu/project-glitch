#pragma once

#include <vector>
#include <algorithm>

template <typename T> class RingBuffer {
private:
    std::vector<T> data;
    size_t windowIndex;
    size_t _capacity;

public:
    RingBuffer(std::size_t size);
    void add(T item);
    T operator[](int index) const;
    size_t length() const;
    size_t capacity() const;
    bool atCapacity() const;
    std::string toString() const;
};

template <typename T> RingBuffer<T>::RingBuffer(size_t size) {
    data = {};
    windowIndex = 0;
    _capacity = size;
}

template <typename T> void RingBuffer<T>::add(T item) {
    if (!atCapacity()) data.push_back(item);
    else {
        data[windowIndex%_capacity] = item;
    }  
    ++windowIndex;
}

template <typename T> T RingBuffer<T>::operator[](int index) const {
    int ii = index % (int)_capacity;
    if (!atCapacity()) return data[(ii + length()) % length()];
    else return data[(windowIndex + ii + _capacity) % _capacity];
}

template <typename T> std::size_t RingBuffer<T>::length() const {
    return std::min(windowIndex, _capacity);
}

template <typename T> size_t RingBuffer<T>::capacity() const {
    return _capacity;
}

template <typename T> bool RingBuffer<T>::atCapacity() const {
    return windowIndex >= _capacity;
}

template <typename T> std::string RingBuffer<T>::toString() const {
    std::stringstream ss;
    ss << "RingBuffer" << (atCapacity() ? "" : "!") << "@capacity:<";
    for (size_t i = 0; i < length(); i++) {
        ss << (*this)[i];
        if (i < length() - 1) ss << ",";
        else ss << ">";
    }
    return ss.str();
}