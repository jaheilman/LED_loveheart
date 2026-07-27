#include "TapDetection.hpp"

namespace {
    const int8_t DECREASING    = -1; 
    const int8_t CONSTANT      = 0;
    const int8_t INCREASING    = 1;
}

TapDetection::TapDetection() {}

int TapDetection::update(uint16_t distance) {
    distances.push(distance);
    return 0;
    //return get_gesture();
}


int TapDetection::get_gesture() {
    if (distances.size() < TAP_BUFFER_SIZE) {
        return Gestures_t::NOT_READY;
    }

    int taps_detected = 0;
    for (size_t i = 1; i < distances.size() - 1; ++i) {
        if ((distances.get(i - 1) > distances.get(i) + tap_change_threshold) &&
            (distances.get(i + 1) > distances.get(i) + tap_change_threshold))
        {
            ++taps_detected;
        }
    }

    return taps_detected;
}

/*
 * A tap is defined as a distance decrease of <tap_threshold> for <tap_duration> samples, then increase
 * A double tap is two single taps in the sample window
 * A long tap is 2*<tap_duration>
 */