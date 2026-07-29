#ifndef _TAPDETECTION_HPP_
#define _TAPDETECTION_HPP_
#include "RingBuffer.hpp"

#define  TAP_BUFFER_SIZE 16
#define  EVENT_BUFFER_SIZE 8

enum Gestures_t{
    NONE                            = 0,    /*!< No gesture detected */
    SINGLE_TAP                      = 1,    /*!< Single tap detected */
    DOUBLE_TAP                      = 2,    /*!< Doubble tap detected */
    LONG_TAP                        = 3,    /*!< Double tap detected */
    // GESTURES_DOUBLE_SWIPE           = 4,    /*!< Double swipe detected : __SWIPES__*/
    // GESTURES_HAND_ENTERING          = 5,    /*!< Hand is entering in device field of view : __LC__ */
    // GESTURES_HAND_LEAVING           = 6,    /*!< Hand is leaving from device field of view : __LC__ */
    // GESTURES_LEVEL_CONTROLLED       = 7,    /*!< Hand is controlling a level from the measured distance : __LC__ */
    // GESTURES_SWIPE_LEFT_RIGHT       = 8,    /*!< Directional swipe from left to right : __DIRSWIPE__ */
    // GESTURES_SWIPE_RIGHT_LEFT       = 9,    /*!< Directional swipe from right to left : __DIRSWIPE__ */
    // GESTURES_FLIP_PAGE_LEFT_RIGHT   = 10,    /*!< Page flip from left to right (hand at 45) : __FLIPPAGE__ */
    // GESTURES_FLIP_PAGE_RIGHT_LEFT   = 11,    /*!< Page flip from right to left (hand at 45) : __FLIPPAGE__ */
    
    NOT_READY                       = -1,   /*!< A gesture start is detected but full gesture detection is not completed (yet) */
    DISCARDED                       = -2,   /*!< Gesture is discarded */
    DISCARDED_TOO_SLOW              = -3,   /*!< Gesture is discarded because it is too slow */
    DISCARDED_TOO_FAST              = -4,   /*!< Gesture is discarded because it is too fast */
};


struct GestureEvent {
    Gestures_t gesture;
    uint32_t tap_index;   // sample index where tap started
    size_t tap_length;  // number of samples in tap
};

class TapDetection {
public:
    TapDetection();
    uint32_t update(uint16_t distance);
    int8_t gesture_available();
    GestureEvent pop_gesture();
    void clear();
    uint16_t tap_change_threshold = 50;   // mm
    //uint16_t abs_threshold = 30;          // mm
    uint8_t  short_tap_threshold = 3;     // number of samples
    bool floating_threshold = false;      // allow floating threshold


// private:
    uint32_t sample_index = 0;
    RingBuffer<uint16_t> distances{TAP_BUFFER_SIZE};  // note {} not () 
    RingBuffer<int>        gesture{EVENT_BUFFER_SIZE};
    RingBuffer<uint32_t>   gesture_index{EVENT_BUFFER_SIZE};
    RingBuffer<size_t>     gesture_length{EVENT_BUFFER_SIZE};
    void detect_gesture();
};

#endif //_TAPDETECTION_HPP_