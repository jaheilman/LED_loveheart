#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>
#include "Proximity_Gesture/tof_gestures.h"
#include "Proximity_Gesture/tof_gestures_DIRSWIPE_1.h"
#include "Proximity_Gesture/tof_gestures_TAP_1.h"

// The number of sensors in your system.
const uint8_t sensorCount = 3;
uint8_t currSample = 0;
// The Arduino pin connected to the XSHUT pin of each sensor.
const uint8_t xshutPins[sensorCount] = { 4, 5, 6 };
VL53L0X sensors[sensorCount];
uint16_t distances[sensorCount];
Gesture_DIRSWIPE_1_Data_t gestureDirSwipeData[sensorCount];
Gesture_TAP_1_Data_t gestureTapData[sensorCount];
int dirswipes[sensorCount];
int taps[sensorCount];


void setup() {
  while (!Serial) {}
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000); // use 400 kHz I2C

  // Disable/reset all sensors by driving their XSHUT pins low.
  for (uint8_t i = 0; i < sensorCount; i++)  {
    pinMode(xshutPins[i], OUTPUT);
    digitalWrite(xshutPins[i], LOW);
  }

  // Enable, initialize, and start each sensor, one by one.
  for (uint8_t i = 0; i < sensorCount; i++)
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

  for (uint8_t i = 0; i < sensorCount; i++) {
    tof_gestures_initTAP_1(&gestureTapData[i]);
    tof_gestures_initDIRSWIPE_1(400, 0, 1000, &gestureDirSwipeData[i]); 
  }

}

void loop() {
  // for (uint8_t i = 0; i < sensorCount; i++)  {
  //   Serial.print(sensors[i].readRangeContinuousMillimeters());
  //   if (sensors[i].timeoutOccurred()) { Serial.print(" TIMEOUT"); }
  //   Serial.print('\t');
  // }
  // Serial.println();

  for (uint8_t i = 0; i < sensorCount; i++) {    
    distances[i] = sensors[i].readRangeContinuousMillimeters();
  }
  for (uint8_t i = 0; i < sensorCount; i++) {    
    taps[i] = sensors[i].readRangeContinuousMillimeters();
  }
  dirswipes[0] = tof_gestures_detectDIRSWIPE_1(distances[0], distances[1], &gestureDirSwipeData[0]);
  dirswipes[1] = tof_gestures_detectDIRSWIPE_1(distances[1], distances[2], &gestureDirSwipeData[1]);
  dirswipes[2] = tof_gestures_detectDIRSWIPE_1(distances[0], distances[2], &gestureDirSwipeData[2]);


  switch(taps[0])
  {
    case GESTURES_SINGLE_TAP:
      Serial.println("GESTURES_SINGLE_TAP DETECTED!!!");
      break;
    default:
      // Do nothing
      break;
  }

  switch(dirswipes[0])
  {
    case GESTURES_SWIPE_LEFT_RIGHT:
      Serial.println("From LEFT to RIGHT --->");
      break;
    case GESTURES_SWIPE_RIGHT_LEFT:
      Serial.println("From RIGHT to LEFT <---");
      break;
    default:
      // Do nothing
      break;
  }

}
