#include <Arduino.h>
#include "CircularBuffer.hpp"


CircularBuffer::CircularBuffer(size_t buffer_capacity) : head(0), tail(0), count(0) {
    buf_capacity = buffer_capacity;
    if (buffer_capacity > CIRCULARBUFFER_MAX_CAPACITY) {
        Serial.println("Ring buffer max exceeded; initializing to max capacity");
        buf_capacity = CIRCULARBUFFER_MAX_CAPACITY;
    }
    // Initialize the buffer with zeros
    for (size_t i = 0; i < CIRCULARBUFFER_MAX_CAPACITY; ++i) {
        buffer[i] = 0;
    }
}

// Add an element to the buffer
void CircularBuffer::push(uint16_t value) {
    if (is_full()) {
        // Serial.println("Buffer overflow");
    }
    buffer[head] = value;
    head = (head + 1) % buf_capacity;
    ++count;
}

// Remove an element from the buffer
uint16_t CircularBuffer::pop() {
    if (is_empty()) {
        Serial.println("Buffer underflow");
        return 0;
    }
    uint16_t value = buffer[tail];
    tail = (tail + 1) % buf_capacity;
    --count;
    return value;
}

uint16_t CircularBuffer::get(size_t index) const {
    if (is_empty()) {
        Serial.println("Circular buffer is empty");
        return 0;
    }
    size_t i = (tail + index) % buf_capacity;
    uint16_t value = buffer[i];
    return value;
}

void CircularBuffer::clear(){
    head = 0;
    tail = 0;
    count = 0;
}

// Check if the buffer is empty
bool CircularBuffer::is_empty() const {
    return count == 0;
}

// Check if the buffer is full
bool CircularBuffer::is_full() const {
    return count == buf_capacity;
}

// Get the current size of the buffer
size_t CircularBuffer::size() const {
    return count;
}

uint16_t CircularBuffer::buf_mean() const {
    if (is_empty()) {
        return 0;
    }
    uint32_t sum = 0;
    for (size_t i = 0; i < size(); ++i) {
        sum += get(i);
    }
    return uint16_t(sum / size());
}

uint16_t CircularBuffer::buf_min() const {
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

uint16_t CircularBuffer::buf_max() const {
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