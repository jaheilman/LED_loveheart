#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>
#include "Proximity_Gesture/tof_gestures.h"
#include "Proximity_Gesture/tof_gestures_DIRSWIPE_1.h"
#include "Proximity_Gesture/tof_gestures_TAP_1.h"
#include <FastLED.h>

// LED SETUP
#define NUM_LEDS 50
#define DATA_PIN 2
#define LED_VOLTS 5
#define LED_MA 500
#define BRIGHTNESS 60
#define CHIPSET WS2812B

CRGB leds[NUM_LEDS];


// VL53L0X SENSOR SETUP
// The number of sensors in your system.
#define SENSOR_COUNT 3
// The Arduino pin connected to the XSHUT pin of each sensor.
const uint8_t xshutPins[SENSOR_COUNT] = { 4, 5, 6 };
VL53L0X sensors[SENSOR_COUNT];
uint16_t distances[SENSOR_COUNT];
Gesture_DIRSWIPE_1_Data_t gestureDirSwipeData[SENSOR_COUNT];
Gesture_TAP_1_Data_t gestureTapData[SENSOR_COUNT];
int dirswipes[SENSOR_COUNT];
int taps[SENSOR_COUNT];
uint16_t update_delay = 500;
int8_t dir = 1;

//segment 5, led 3 is missing
uint8_t num_segments = 11;
uint8_t led_segments[] = {1, 2, 3, 4, 4, 6, 7, 6, 7, 6, 4};
// pretent the heart is on a 13 x 11 grid?


void march();

void setup() {
  while (!Serial) {}
  Serial.begin(115200);
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

  // Disable/reset all sensors by driving their XSHUT pins low.
  for (uint8_t i = 0; i < SENSOR_COUNT; i++)  {
    pinMode(xshutPins[i], OUTPUT);
    digitalWrite(xshutPins[i], LOW);
  }

  // Enable, initialize, and start each sensor, one by one.
  for (uint8_t i = 0; i < SENSOR_COUNT; i++)
  {
    // Stop driving this sensor's XSHUT low. This should allow the carrier
    // board to pull it high. (We do NOT want to drive XSHUT high since it is
    // not level shifted.) Then wait a bit for the sensor to start up.
    pinMode(xshutPins[i], INPUT);
    delay(10);
    sensors[i].setTimeout(500);
    if (!sensors[i].init()) {
      Serial.print("Failed to detect and initialize sensor ");
      Serial.println(i);
      while (1);
    }
    // Give each sensor a unique address, incrementing from  0x2A.
    sensors[i].setAddress(0x2A + i);
    sensors[i].startContinuous(50);
  }

  for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
    tof_gestures_initTAP_1(&gestureTapData[i]);
    tof_gestures_initDIRSWIPE_1(400, 0, 1000, &gestureDirSwipeData[i]); 
  }

}

void loop() {
  // for (uint8_t i = 0; i < SENSOR_COUNT; i++)  {
  //   Serial.print(sensors[i].readRangeContinuousMillimeters());
  //   if (sensors[i].timeoutOccurred()) { Serial.print(" TIMEOUT"); }
  //   Serial.print('\t');
  // }
  // Serial.println();

  for (uint8_t i = 0; i < SENSOR_COUNT; i++) {    
    distances[i] = sensors[i].readRangeContinuousMillimeters();
  }
  for (uint8_t i = 0; i < SENSOR_COUNT; i++) {    
    taps[i] = sensors[i].readRangeContinuousMillimeters();
  }
  dirswipes[0] = tof_gestures_detectDIRSWIPE_1(distances[0], distances[1], &gestureDirSwipeData[0]);
  dirswipes[1] = tof_gestures_detectDIRSWIPE_1(distances[1], distances[2], &gestureDirSwipeData[1]);
  dirswipes[2] = tof_gestures_detectDIRSWIPE_1(distances[0], distances[2], &gestureDirSwipeData[2]);

  if (taps[0] == GESTURES_SINGLE_TAP ){
  }
  if (taps[1] == GESTURES_SINGLE_TAP ){
  }
  if (taps[2] == GESTURES_SINGLE_TAP ){
  }
  // if (taps[0] == GESTURES_DOUBLE_TAP ){
  // }

  
  if (dirswipes[0] == GESTURES_SWIPE_LEFT_RIGHT && dirswipes[1] == GESTURES_SWIPE_LEFT_RIGHT){
    // action RIGHT
    dir = 1;
  }
  if (dirswipes[0] == GESTURES_SWIPE_RIGHT_LEFT && dirswipes[1] == GESTURES_SWIPE_RIGHT_LEFT){
    // action LEFT
    dir = -1;
  }
  if (dirswipes[0] == GESTURES_SWIPE_LEFT_RIGHT && dirswipes[1] == GESTURES_SWIPE_RIGHT_LEFT){
    // action DOWN
    update_delay -= 100;
    if (update_delay < 100) {
      update_delay = 100;
    }
  }
  if (dirswipes[0] == GESTURES_SWIPE_RIGHT_LEFT && dirswipes[1] == GESTURES_SWIPE_LEFT_RIGHT){
    // action UP
    update_delay += 100;
    if (update_delay > 2000){
      update_delay = 2000;
    }
  }


  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Red;
  }

  march();
  FastLED.show();
  FastLED.delay(update_delay);
}

void march(){
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
      
  }

}