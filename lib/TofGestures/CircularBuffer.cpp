#include <Arduino.h>
#include "CircularBuffer.hpp"


RingBuffer::RingBuffer() : head(0), tail(0), count(0) {
    // Initialize the buffer with zeros
    for (size_t i = 0; i < SIZE; ++i) {
        buffer[i] = 0;
    }
}

// Add an element to the buffer
void RingBuffer::push(uint16_t value) {
    if (is_full()) {
        // Serial.println("Circular buffer is full");
    }
    buffer[head] = value;
    head = (head + 1) % SIZE;
    ++count;
}

// Remove an element from the buffer
uint16_t RingBuffer::pop() {
    if (is_empty()) {
        Serial.println("Circular buffer is empty");
        return 0;
    }
    uint16_t value = buffer[tail];
    tail = (tail + 1) % SIZE;
    --count;
    return value;
}

uint16_t RingBuffer::get(size_t index) const {
    if (is_empty()) {
        Serial.println("Circular buffer is empty");
        return 0;
    }
    size_t i = (tail + index) % SIZE;
    uint16_t value = buffer[i];
    return value;
}

// Check if the buffer is empty
bool RingBuffer::is_empty() const {
    return count == 0;
}

// Check if the buffer is full
bool RingBuffer::is_full() const {
    return count == SIZE;
}

// Get the current size of the buffer
size_t RingBuffer::size() const {
    return count;
}

uint16_t RingBuffer::buf_mean() const {
    if (is_empty()) {
        return 0;
    }
    uint32_t sum = 0;
    for (size_t i = 0; i < size(); ++i) {
        sum += get(i);
    }
    return uint16_t(sum / size());
}

uint16_t RingBuffer::buf_min() const {
    if (is_empty()) {
        return 0;
    }
    uint16_t minval = UINT16_MAX;
    for (size_t i = 0; i < size(); ++i) {
        if (get(i) < minval) {
            minval = get(i);
        }
    }
    return minval;
}

uint16_t RingBuffer::buf_max() const {
    if (is_empty()) {
        return 0;
    }
    uint16_t maxval = 0;
    for (size_t i = 0; i < size(); ++i) {
        if (get(i) > maxval) {
            maxval = get(i);
        }
    }
    return maxval;
}