#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <FastLED.h>
#include "TapDetection.hpp"


//#define _TEST 1
#ifdef _TEST
#include "../lib/TofGestures/Test_TofGestures.hpp"
#endif

#define DISTACE_SENSORS 1


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
uint16_t rainbow_counter = 0;


// VL53L0X SENSOR SETUP
// The number of sensors in your system.
#define SENSOR_COUNT 3
VL53L0X sensors[SENSOR_COUNT];
const uint8_t xshutPins[SENSOR_COUNT] = { 6, 7, 8 }; //s, 
uint16_t update_delay = 200;

uint16_t distances[SENSOR_COUNT];
TapDetection Taps[SENSOR_COUNT];

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

  // Enable, initialize, and start each sensor, one by one.
  if (DISTACE_SENSORS) {
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
        Serial.print("Failed to detect and initialize sensor ");
        Serial.print(i);
        Serial.print("Halting.");
        while (1);
      }
      Serial.print("Initializing sensor  ");
      Serial.println(i);
      sensors[i].setAddress(0x2A + i);
      sensors[i].setTimeout(update_delay/2);
    }
    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
      Serial.print("Sensor "); Serial.print(i); Serial.println(":"); 
      Serial.print("Measurement timing budget: ");
      Serial.println(sensors[i].getMeasurementTimingBudget());
      Serial.print("Timeout: ");
      Serial.println(sensors[i].getTimeout());
      sensors[i].startContinuous(update_delay);
    }
    Serial.println("VL53 sensors initialized.");
  }

  for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
    
  }

}

void loop() {
  for (uint8_t i = 0; i < SENSOR_COUNT; i++)  {
    distances[i] = sensors[i].readRangeContinuousMillimeters();
    if (sensors[i].timeoutOccurred()) { 
      distances[i] = 501; 
    }
    if (distances[i] < 20) {
      distances[i] = 500;
    }
    Taps[i].update(distances[i]);
  }

  if (DISTACE_SENSORS){
    for (uint8_t i = 0; i < SENSOR_COUNT; i++)  {
      Serial.print(distances[i]);
      Serial.print('\t');
    }
  }
  Serial.println();

  for (uint8_t i = 0; i < SENSOR_COUNT; i++)  {
    if (Taps[i].get_gesture() > 0) {
      Serial.print("Tap detected on sensor "); Serial.println(i);
    }

  }
 
  static int led_indx = 0;
  static CRGB mycolor = CRGB::Red;
  leds[led_indx] = mycolor;
  if (++led_indx > NUM_LEDS) {
    led_indx = 0;
    if (mycolor == CRGB::Red){
      mycolor = CRGB::Green;
    } else {
      mycolor = CRGB::Red;
    }
  }
  // for (uint8_t i = 0; i < NUM_LEDS; i++) {
  //   leds[i] = CRGB::Red;
  // }
  // march();
  FastLED.show();
  FastLED.delay(update_delay);
}

void march(){
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
      
  }

}

void rainbowCycle(void){
  const uint8_t hue = 8;
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
      leds[i].r = hue*i*(rainbow_counter + i + 0  ) % 256;
      leds[i].g = hue*i*(rainbow_counter + i + 64 ) % 256;
      leds[i].b = hue*i*(rainbow_counter + i + 128) % 256;
  }
  ++rainbow_counter;
}