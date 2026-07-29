#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <FastLED.h>
#include "TapDetection.hpp"


//#define _TEST 1
#ifdef _TEST
#include "../lib/TofGestures/Test_TofGestures.hpp"
#endif

#define ENABLE_DISTANCE_SENSORS 1


// LED SETUP
#define NUM_LEDS 50
#define DATA_PIN 4
#define LED_VOLTS 5
#define LED_MA 500
#define BRIGHTNESS 60
#define CHIPSET WS2812B
CRGB leds[NUM_LEDS];

uint8_t led_segments[] = {1, 2, 3, 4, 4, 6, 7, 6, 7, 6, 4}; //segment 5, led 3 is missing
uint8_t num_segments = sizeof(led_segments) / sizeof(led_segments[0]);
CRGB color_wheel[] = {
  CRGB::Red,
  CRGB::Yellow,
  CRGB::Lime,
  //CRGB::Green,
  CRGB::Aqua,
  CRGB::Blue,
  CRGB::Fuchsia,
  // CRGB::Purple,
  CRGB::Violet,
  CRGB::White,
  CRGB::Black,
};
uint8_t color_wheel_size = sizeof(color_wheel) / sizeof(color_wheel[0]);
int8_t  wheel_index = 0;

// VL53L0X SENSOR SETUP
// The number of sensors in your system.
#define SENSOR_COUNT 3
VL53L0X sensors[SENSOR_COUNT];
const uint8_t xshutPins[SENSOR_COUNT] = { 6, 7, 8 }; //s, 
uint16_t sample_delay = 50;
const uint8_t update_interval = 1;
uint8_t update_index = 0;
TapDetection Taps[SENSOR_COUNT];
size_t tap_buffer_size = Taps[0].distances.capacity();
uint16_t dist = 0;
// Multi-sensor gesture
int complex_gesture(/*Taps*/);


// Serial buffer
char buffer[50];


void setup() {
  while (!Serial) {}
  Serial.begin(115200);
  Serial.println("Loveheart 0.1");

#ifdef _TEST
  Serial.println("Running tests...");
  TEST_RingBuffer();
  Serial.println("... Tests complete!");
  while(1);
#endif


  Wire.begin();
  Wire.setClock(400000); // use 400 kHz I2C

  // Initialize the LED strip.
  FastLED.addLeds<CHIPSET, DATA_PIN, GRB>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip)
    .setDither(BRIGHTNESS < 255);
  FastLED.setMaxPowerInVoltsAndMilliamps(LED_VOLTS, LED_MA);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  FastLED.show();
  Serial.println("FastLED initialized.");

  // Enable, initialize, and start each sensor, one by one.
  if (ENABLE_DISTANCE_SENSORS) {
      // Disable/reset all sensors by driving their XSHUT pins low.
    for (uint8_t i = 0; i < SENSOR_COUNT; i++)  {
      pinMode(xshutPins[i], OUTPUT);
      digitalWrite(xshutPins[i], LOW);
    }
    delay(100);
    for (uint8_t i = 0; i < SENSOR_COUNT; i++)
    {
      // Stop driving this sensor's XSHUT low. This should allow the carrier
      // board to pull it high. (We do NOT want to drive XSHUT high since it is
      // not level shifted.) Then wait a bit for the sensor to start up.
      pinMode(xshutPins[i], INPUT);
      delay(10);
      sensors[i].setTimeout(500);
      if (!sensors[i].init()) {
        sprintf(buffer,"FAIL INIT sensor %d, halting.", i);
        Serial.print(buffer);
        while (1);
      }
      Serial.print("Initializing sensor  ");
      Serial.println(i);
      sensors[i].setAddress(0x2A + i);
      sensors[i].setTimeout(sample_delay/2);
    }
    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
      Serial.print("Sensor "); Serial.print(i); Serial.println(":"); 
      Serial.print("Measurement timing budget: ");
      Serial.println(sensors[i].getMeasurementTimingBudget());
      Serial.print("Timeout: ");
      Serial.println(sensors[i].getTimeout());
      sensors[i].startContinuous(sample_delay);
    }
    Serial.println("VL53 sensors initialized.");
  }

}

void loop() {

  static int led_indx = 0;
  static int led_advance = 1;
  static CRGB mycolor = color_wheel[wheel_index % color_wheel_size];

  static int8_t check_gesture = 0;
  
  for (size_t i = 0; i < SENSOR_COUNT; i++)  {
    dist = sensors[i].readRangeContinuousMillimeters();
    if (sensors[i].timeoutOccurred()) { 
      dist = 202; 
    }
    if (dist > 202) { 
      dist = 201; 
    }
    if (dist < 20) {
      dist = 200;
    }
    Taps[i].update(dist);
  }
  
  // Check if there is a tap on any sensor.
  // If so, reset the check_delay counter to CHECK_DELAY.
  // This allows the other sensors to register gestures before
  // testing for a swipe.
  for (uint8_t i = 0; i  < SENSOR_COUNT; i++)  {
    // if (Taps[i].get_gesture() > 0) {
    //   Serial.print("Tap detected on sensor "); Serial.println(i);
    // }
    check_gesture += Taps[i].gesture_available();
  }

  if (check_gesture > 2) {
    check_gesture = 0;
    int swipe = complex_gesture();
  
    if (swipe > 0) {
      Serial.print("SWIPE: "); Serial.println(swipe);
    }
    if (swipe == 1) { // right
      led_advance = 1;
      mycolor = color_wheel[++wheel_index % color_wheel_size];
    }
    else if (swipe == 2) { // left
      led_advance = 1;
      mycolor = color_wheel[(wheel_index-2) % color_wheel_size];
    }
    else if (swipe == 3) { // up-right
      led_advance = 1;
      mycolor = color_wheel[++wheel_index % color_wheel_size];
    }
    else if (swipe == 4) { // down-left
      led_advance = -1;
      mycolor = color_wheel[(wheel_index-2) % color_wheel_size];
    }
    else if (swipe == 5) { // up-left
      led_advance = 1;
      mycolor = color_wheel[++wheel_index % color_wheel_size];
    }
    else if (swipe == 6) { // down-right
      led_advance = -1;
      mycolor = color_wheel[(wheel_index-2) % color_wheel_size];
    }
    if (swipe > 0) {
      Serial.print("Wheel index (swipe): "); Serial.println(wheel_index);
      swipe = 0;
    }

    for (size_t i = 0; i < SENSOR_COUNT; i++) {
      Taps[i].clear();
    }
  }

  leds[led_indx] = mycolor;
  led_indx += led_advance;
  if (led_indx >= NUM_LEDS) {
    led_indx = 0;
    mycolor = color_wheel[++wheel_index % color_wheel_size];
    Serial.print("Wheel index (rollover): "); Serial.println(wheel_index);
  }
  if (led_indx < 0) {
    led_indx = NUM_LEDS - 1;
    mycolor = color_wheel[++wheel_index % color_wheel_size];
    Serial.print("Wheel index (rollunder): "); Serial.println(wheel_index);
  }

  if (wheel_index < 0) {
    wheel_index = color_wheel_size - 1;
  }
  else if (wheel_index >= color_wheel_size) {
    wheel_index = 0;
  }

  FastLED.show();
  FastLED.delay(sample_delay);
}



int complex_gesture(){
  // 0 = Nothing
  // 1 = swipe right
  // 2 = swipe left
  // 3 = swipe up-right
  // 4 = swipe down-left
  // 5 = swipe up-left
  // 6 = swipe down-right

  GestureEvent G[SENSOR_COUNT];
  // uint32_t tap_indexes[SENSOR_COUNT];
  // size_t tap_lengths[SENSOR_COUNT];

  for (size_t i = 0; i < SENSOR_COUNT; i++) {
    G[i] = Taps[i].pop_gesture();
  }

  if ((G[0].tap_index < G[1].tap_index) && (G[1].tap_index < G[2].tap_index) && (G[2].tap_index != TAP_BUFFER_SIZE + 1)) {
    return 1; // swipe right
  } 
  else if ((G[2].tap_index < G[1].tap_index) && (G[1].tap_index < G[0].tap_index) && (G[0].tap_index != TAP_BUFFER_SIZE + 1)) {
    return 2; // swipe left
  } 
  else if (G[1].tap_length < G[2].tap_length) {
    return 3; // swipe up-right
  } 
  else if (G[1].tap_length > G[2].tap_length) {
    return 4; // swipe down-left
  } 
  else if (G[1].tap_length < G[0].tap_length) {
    return 5; // swipe up-left
  } 
  else if (G[1].tap_length > G[0].tap_length) {
    return 6; // swipe down-right
  } 
  else {
    return 0; // no gesture detected
  }

}