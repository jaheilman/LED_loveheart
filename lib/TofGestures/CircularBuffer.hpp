#include <Arduino.h>

class RingBuffer {
public:
    static constexpr size_t SIZE = 16; // Fixed size for the ring buffer

    RingBuffer();

    void push(uint16_t value);          // Add an element to the buffer
    uint16_t pop();                     // Remove oldest element from the buffer
    uint16_t get(size_t index) const;   // Get an element at a index (relative to head)
    bool is_empty() const;
    bool is_full() const;
    size_t size() const;
    uint16_t buf_mean() const;
    uint16_t buf_min() const;
    uint16_t buf_max() const;

private:
    uint16_t buffer[SIZE];
    size_t head; // Points to the next insertion position
    size_t tail; // Points to the next removal position
    size_t count; // Tracks the number of elements in the buffer
};
