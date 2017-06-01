/*
 *  Battery-powered MySensors-2.x sensor
 */
#include <Arduino.h>
#include <Vcc.h>

#define MY_DEBUG
#define MY_RADIO_NRF24
#define MY_BAUD_RATE 9600
#include <MySensors.h>

#define DEBUG

#define SKETCH_NAME "Battery Sensor"
#define SKETCH_MAJOR_VER "0"
#define SKETCH_MINOR_VER "4"
#define BATTERY_SENSOR 0

// unsigned uint32_t = 24*60*60*1000; // h*min*sec*1000
uint32_t SLEEP_TIME = 60 * 1000L; // 60s = 1min
uint8_t unusedPins[] = {2, 3, 4, 5, 6, 7, 8};

const float VccMin   = 1.8;           // Minimum expected Vcc level, in Volts.
const float VccMax   = 3.3;           // Maximum expected Vcc level, in Volts.
const float VccCorrection = 3.23/3.26;      // Measured Vcc by multimeter divided by reported Vcc

Vcc vcc(VccCorrection);

// Globals
float oldBatPercentage;
// MySensors messages
MyMessage vMsg(BATTERY_SENSOR, V_VOLTAGE);

/*
 * MySensors 2.x presentation
 */
void presentation() {
#ifdef DEBUG
  Serial.println("presentation");
#endif
  sendSketchInfo(SKETCH_NAME, SKETCH_MAJOR_VER "." SKETCH_MINOR_VER);
  present(BATTERY_SENSOR, S_MULTIMETER, "Battery Voltage");
}

/*
 * Setup
 */
void setup()
{
#ifdef DEBUG
  Serial.println("setup");
#endif
  // Reset unused pins
  int count = sizeof(unusedPins)/sizeof(int);
  for (int i = 0; i < count; i++) {
    pinMode(unusedPins[i], INPUT);
    digitalWrite(unusedPins[i], LOW);
  }
  oldBatPercentage = -1;
}

/*
 * Send sensor and battery values
 */
void sendValues()
{
#ifdef DEBUG
  Serial.println("sendValues");
#endif
  // Send sensor values
  // ...

  // Battery voltage
  float volts = vcc.Read_Volts();
#ifdef DEBUG
  Serial.print("VCC (volts) = ");
  Serial.println(volts);
#endif
  send(vMsg.set(volts, 3));

  // Battery percentage
  uint8_t p = (uint8_t)round(vcc.Read_Perc(VccMin, VccMax));
  #ifdef DEBUG
    Serial.print("VCC (percentage) = ");
    Serial.println(p);
  #endif
  if (oldBatPercentage != p) {
    sendBatteryLevel(p);
    oldBatPercentage = p;
  }
}

/*
 * Loop
 */
void loop() {
#ifdef DEBUG
  Serial.println("loop");
#endif
  if (oldBatPercentage == -1) { // first start
    // Send the values before sleeping
    sendValues();
  }
  sleep(SLEEP_TIME);
  // Read sensors and send on wakeup
  sendValues();
}
