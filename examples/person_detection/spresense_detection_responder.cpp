/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#if defined(ARDUINO) && !defined(ARDUINO_ARCH_SPRESENSE)
#define ARDUINO_EXCLUDE_CODE
#endif  // defined(ARDUINO) && !defined(ARDUINO_ARCH_SPRESENSE)

#ifndef ARDUINO_EXCLUDE_CODE

#include "detection_responder.h"

#include "Arduino.h"

// Flash the blue LED after each inference
void RespondToDetection(tflite::ErrorReporter* error_reporter,
                        int8_t person_score, int8_t no_person_score) {
  static bool is_initialized = false;
  if (!is_initialized) {
    // Pins for the built-in RGB LEDs on the Arduino Nano 33 BLE Sense
    pinMode(LED_BUILTIN0, OUTPUT);
    pinMode(LED_BUILTIN1, OUTPUT);
    pinMode(LED_BUILTIN2, OUTPUT);
    is_initialized = true;
  }

  // Note: The RGB LEDs on the Arduino Nano 33 BLE
  // Sense are on when the pin is LOW, off when HIGH.

  // Switch the person/not person LEDs off
  digitalWrite(LED_BUILTIN0, LOW);
  digitalWrite(LED_BUILTIN1, LOW);

  // Flash the blue LED after every inference.
  digitalWrite(LED_BUILTIN2, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN2, HIGH);

  // Switch on the LED1 when a person is detected,
  // the LED0 when no person is detected
  if (person_score > no_person_score) {
    digitalWrite(LED_BUILTIN0, LOW);
    digitalWrite(LED_BUILTIN1, HIGH);
  } else {
    digitalWrite(LED_BUILTIN0, HIGH);
    digitalWrite(LED_BUILTIN1, LOW);
  }

  TF_LITE_REPORT_ERROR(error_reporter, "Person score: %d No person score: %d",
                       person_score, no_person_score);
}

#endif  // ARDUINO_EXCLUDE_CODE
