
#include "CircularBuffer.hpp"

void TEST_RingBuffer() {
    // Test Ring buffer
    RingBuffer rb;

    for (int i = 0; i < 10; i++) {
        rb.push(1);
    }
    Serial.print("Test RB size() - should be 10: ");
    Serial.println(rb.size());
    if (rb.size() == 10) {
        Serial.println("PASSED RB size()");
    } else {
        Serial.println("FAILED RB size()");
    }

    Serial.print("Test RB buf_mean() - should be 1: ");
    Serial.println(rb.buf_mean());
    if (rb.buf_mean() == 1) {
        Serial.println("PASSED buf_mean()");
    } else {
        Serial.println("FAILED RB buf_mean()");
    }

    rb.push(2);
    Serial.print("Test RB buf_max() - should be 2: ");
    Serial.println(rb.buf_max());
    if (rb.buf_max() == 2) {
        Serial.println("PASSED buf_mean()");
    } else {
        Serial.println("FAILED RB buf_mean()");
    }

    Serial.print("Test RB buf_min() - should be 1: ");
    Serial.println(rb.buf_min());
    if (rb.buf_min() == 1) {
        Serial.println("PASSED buf_min()");
    } else {
        Serial.println("FAILED RB buf_min()");
    }

    rb.push(0);
    Serial.print("Test RB buf_min() - should be 0: ");
    Serial.println(rb.buf_min());
    if (rb.buf_min() == 0) {
        Serial.println("PASSED buf_min()");
    } else {
        Serial.println("FAILED RB buf_min()");
    }
}

void TEST_TepDetection() {

}