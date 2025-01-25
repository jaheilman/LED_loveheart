#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <FastLED.h>

//#define _TEST 1
#ifdef _TEST
#include "../lib/TofGestures/Test_TofGestures.hpp"
#endif

// LED SETUP
#define NUM_LEDS 50
#define DATA_PIN 4
#define LED_VOLTS 5
#define LED_MA 500
#define BRIGHTNESS 60
#define CHIPSET WS2812B
CRGB leds[NUM_LEDS];
uint8_t num_segments = 11;
uint8_t led_segments[] = {1, 2, 3, 4, 4, 6, 7, 6, 7, 6, 4}; //segment 5, led 3 is missing


// VL53L0X SENSOR SETUP
// The number of sensors in your system.
#define SENSOR_COUNT 3
VL53L0X sensors[SENSOR_COUNT];
const uint8_t xshutPins[SENSOR_COUNT] = { 6, 7, 8 }; //s, 
uint16_t update_delay = 500;




void march();

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

  // Disable/reset all sensors by driving their XSHUT pins low.
  for (uint8_t i = 0; i < SENSOR_COUNT; i++)  {
    pinMode(xshutPins[i], OUTPUT);
    digitalWrite(xshutPins[i], LOW);
  }
  delay(100);

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
      Serial.print(i);
      Serial.print("Halting.");
      while (1);
    }
    // Give each sensor a unique address, incrementing from  0x2A.
    sensors[i].setAddress(0x2A + i);
    Serial.print("Initialized sensor  ");
    Serial.println(i);
  }
  for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
    Serial.print("Sensor "); Serial.print(i); Serial.println(":"); 
    Serial.print("Measurement timing budget: ");
    Serial.println(sensors[i].getMeasurementTimingBudget());
    Serial.print("Timeout: ");
    Serial.println(sensors[i].getTimeout());
    sensors[i].startContinuous(100);
  }
  // for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
  //   tof_gestures_initTAP_1(&gestureTapData[i]);
  //   tof_gestures_initDIRSWIPE_1(400, 0, 1000, &gestureDirSwipeData[i]); 
  // }
  Serial.println("VL53 sensors initialized.");

}

void loop() {
  for (uint8_t i = 0; i < SENSOR_COUNT; i++)  {
    Serial.print(sensors[i].readRangeContinuousMillimeters());
    if (sensors[i].timeoutOccurred()) { Serial.print("TMOUT"); }
    Serial.print('\t');
  }
  Serial.println();

  // for (uint8_t i = 0; i < NUM_LEDS; i++) {
  //   leds[i] = CRGB::Red;
  // }
  // march();
  // FastLED.show();
  FastLED.delay(update_delay);
}

void march(){
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
      
  }

}