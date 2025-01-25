#include "TapDetection.hpp"

namespace {
    const int8_t DECREASING    = -1; 
    const int8_t CONSTANT      = 0;
    const int8_t INCREASING    = 1;
}

TapDetection::TapDetection() {

}

int TapDetection::update(uint16_t distance) {
    distances.push(distance);
    return get_gesture();
}

int TapDetection::get_gesture() {
    if (distances.size() < 16) {
        return Gestures_t::NOT_READY;
    }
    // todo: actual gesture logic
    uint16_t tap_duration = 0;
    int8_t direction = CONSTANT;
    for (size_t i = 1; i < distances.size(); i++) {
        if (distances.get(i - 1) > distances.get(i)) {
            if (direction == DECREASING) {
                ++tap_duration;
            }
            direction = DECREASING; 
        }
        else if (distances.get(i - 1) < distances.get(i)) { 
            direction = INCREASING; 
        } 
        else { 
            direction = CONSTANT; 
        }
    }


    return distances.get(0);
}

/*
 * A tap is defined as a distance decrease of <tap_threshold> for <tap_duration> samples, then increase
 * A double tap is two single taps in the sample window
 * A long tap is 2*<tap_duration>
 */