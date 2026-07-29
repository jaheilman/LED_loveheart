#include "TapDetection.hpp"

TapDetection::TapDetection() {}

void TapDetection::detect_gesture() {
    
    if (distances.size() < TAP_BUFFER_SIZE) {
        return;
    }

    int tap_detected = 0;
    size_t tap_index = 0;
    size_t tap_length = 0;
    for (size_t i = 1; i < distances.size() - 2; ++i) {
        // Look for the start of a tap (falling edge)
        if (distances.get(i - 1) > distances.get(i) + tap_change_threshold) {
            tap_length = 1;
            // Count how long the distance remains low
            while (tap_length <= short_tap_threshold       &&
                   (i + tap_length) < distances.size() - 1 &&
                   distances.get(i + tap_length) <= distances.get(i) + tap_change_threshold
            ) {
                tap_length++;
            }

            // Check for rising edge after the tap
            if ((i + tap_length) < distances.size() &&
                distances.get(i + tap_length) > distances.get(i) + tap_change_threshold
            ) {
                tap_detected = 1;
                tap_index = i;
                break;
            }
        }
    }

    if (tap_detected) {
        gesture.push(Gestures_t::SINGLE_TAP);
        gesture_index.push(sample_index + tap_index);
        gesture_length.push(tap_length);
    }

    return;
}
void TapDetection::clear() {
    distances.clear();
    gesture.clear();
    gesture_index.clear();
    gesture_length.clear();
    sample_index = 0;
}


uint32_t TapDetection::update(uint16_t distance) {
    sample_index++;
    distances.push(distance);
    detect_gesture();
    return sample_index;
}

GestureEvent TapDetection::pop_gesture() {
    if (distances.size() < TAP_BUFFER_SIZE) {
        return GestureEvent{
            Gestures_t::NOT_READY,
            0,
            0
        };
    }
    if (gesture.is_empty()) {
        return GestureEvent{
            Gestures_t::NONE,
            0,
            0
        };
    }
    return GestureEvent{
        (Gestures_t)gesture.pop(),
        gesture_index.pop(),
        gesture_length.pop()
    };
}

int8_t TapDetection::gesture_available(){
    return !gesture.is_empty();
}