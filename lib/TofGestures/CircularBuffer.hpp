#include <Arduino.h>
#define CIRCULARBUFFER_MAX_CAPACITY 16

class CircularBuffer {
public:
    CircularBuffer(size_t capacity = CIRCULARBUFFER_MAX_CAPACITY);
    void push(uint16_t value);          // Add an element to the buffer
    uint16_t pop();                     // Remove oldest element from the buffer
    uint16_t get(size_t index) const;   // Get an element at a index (relative to head)
    void clear();
    bool is_empty() const;
    bool is_full() const;
    size_t size() const;                // buffer used
    size_t capacity() const;            // max buffer size 
    uint16_t buf_mean() const;
    uint16_t buf_min() const;
    uint16_t buf_max() const;


private:
    uint16_t buffer[CIRCULARBUFFER_MAX_CAPACITY];
    size_t head; // Points to the next insertion position
    size_t tail; // Points to the next removal position
    size_t count; // Tracks the number of elements in the buffer
    size_t buf_capacity;
};
