#ifndef _RINGBUFFER_HPP_
#define _RINGBUFFER_HPP_

/* A ring buffer implementation that can be created with arbitrary simple type and size
 * Ex:
 * RingBuffer<int> int_rb(10);
 * RingBuffer<float> float_rb(23);
 * 
 * Don't try to send it objects or anything that passes by reference or pointer
 */

#include <Arduino.h>
template <typename T>
class RingBuffer {
private:
    T* buffer;
    size_t head; // Points to the next insertion position
    size_t tail; // Points to the next removal position
    size_t count; // Tracks the number of elements in the buffer
    size_t buf_capacity;

public:
    RingBuffer(size_t buffer_capacity) : head(0), tail(0), count(0), buf_capacity(buffer_capacity) {
        buffer = new T[buf_capacity];
        for (size_t i = 0; i < buf_capacity; ++i) {
            buffer[i] = 0;
        }
    }

    // Add an element to the buffer
    bool push(T value) {
        // if (is_full()) {
        //     // Serial.println("Buffer overflow");
        //     return false;
        // }
        buffer[head] = value;
        head = (head + 1) % buf_capacity;
        ++count;
        return true;
    }

    // Remove an element from the buffer
    T pop() {
        if (is_empty()) {
            Serial.println("Buffer underflow");
            return 0;
        }
        uint16_t value = buffer[tail];
        tail = (tail + 1) % buf_capacity;
        --count;
        return value;
    }

    T get(size_t index) const {
        if (is_empty()) {
            Serial.println("Circular buffer is empty");
            return 0;
        }
        size_t i = (tail + index) % buf_capacity;
        uint16_t value = buffer[i];
        return value;
    }

    // Get the value at a specific index without regarding the head and tail positions
    T get_raw(size_t index) const {
        if (is_empty()) {
            Serial.println("Circular buffer is empty");
            return 0;
        }
        if ((index >= buf_capacity)) {
            Serial.println("Index out of bounds");
            return 0;
        }
        uint16_t value = buffer[index];
        return value;
    }

    void clear() {
        head = 0;
        tail = 0;
        count = 0;
    }

    // Check if the buffer is empty
    bool is_empty() const {
        return count == 0;
    }

    // Check if the buffer is full
    bool is_full() const {
        return count == buf_capacity;
    }

    // Get the current size of the buffer
    size_t size() const {
        return count;
    }

    // max buffer size 
    size_t capacity() const {
        return buf_capacity;
    }           

    T buf_mean() const {
        if (is_empty()) {
            return 0;
        }
        T sum = 0;
        for (size_t i = 0; i < size(); ++i) {
            sum += get(i);
        }
        return T(sum / size());
    }

    T buf_min() const {
        if (is_empty()) {
            return 0;
        }
        T minval = get(0); 
        for (size_t i = 0; i < size(); ++i) {
            if (get(i) < minval) {
                minval = get(i);
            }
        }
        return minval;
    }

    T buf_max() const {
        if (is_empty()) {
            return 0;
        }
        T maxval = get(0); 
        for (size_t i = 0; i < size(); ++i) {
            if (get(i) > maxval) {
                maxval = get(i);
            }
        }
        return maxval;
    }
};


#endif // _RINGBUFFER_HPP_