#include <LedControl.h>
#include <Arduino_LED_Matrix.h>
#include "frames.h"
// Define the pins for MAX7219 module
int DIN_PIN = 11;   // Data input
int CLK_PIN = 12;   // Clock
int CS_PIN = 10;    // Chip select
int NUM_DEVICES = 1; // Number of MAX7219 devices connected


ArduinoLEDMatrix matrix;

LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, NUM_DEVICES);

// Define button pins
int startButtonPin = 2;
int resetButtonPin = 3;
int toggleButtonPin = 5; // Pin for the toggle button

// Define buzzer pin
int buzzerPin = 4;

unsigned long previousMillis = 0;
const long interval = 1000; // Update display every 1 second

unsigned long startTime = 0;
unsigned long countdownDuration = 5 * 60 * 1000; // Initial duration: 5 minutes in milliseconds
bool isCountingDown = false;

bool is25Minutes = false; // Flag to track the timer duration

void setup() {
  lc.shutdown(0, false); // Wake up the MAX7219
  lc.setIntensity(0, 8); // Set the brightness (0-15)
  lc.clearDisplay(0);
  pinMode(LED_BUILTIN, OUTPUT);
  matrix.begin();

  pinMode(startButtonPin, INPUT_PULLUP);
  pinMode(resetButtonPin, INPUT_PULLUP);
  pinMode(toggleButtonPin, INPUT_PULLUP); // Toggle button as an input with pull-up

  pinMode(buzzerPin, OUTPUT);

  // Display the initial countdown time
  displayTime(countdownDuration);
}

void loop() {
  unsigned long currentMillis = millis();

  // Check if the start button is pressed
  if (digitalRead(startButtonPin) == LOW && !isCountingDown) {
    startTime = currentMillis;
    isCountingDown = true;
  }

  // Check if the reset button is pressed
  if (digitalRead(resetButtonPin) == LOW) {
    isCountingDown = false;
    countdownDuration = is25Minutes ? 25 * 60 * 1000 : 5 * 60 * 1000; // Reset to selected duration
    displayTime(countdownDuration);
  }

  // Check if the toggle button is pressed to switch timer duration
  if (digitalRead(toggleButtonPin) == LOW && !isCountingDown) {
    delay(100);
    is25Minutes = !is25Minutes; // Toggle between 25 and 5 minutes
    countdownDuration = is25Minutes ? 25 * 60 * 1000 : 5 * 60 * 1000;
    matrix.loadFrame(chip);
    delay(100);
    displayTime(countdownDuration);
  }

  // Check if the timer is counting down
  if (isCountingDown) {
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      if (countdownDuration >= interval) {
        digitalWrite(LED_BUILTIN, HIGH);
        countdownDuration -= interval;
        displayTime(countdownDuration);
      }
      digitalWrite(LED_BUILTIN, LOW);
    }

    if (countdownDuration == 0) {
      // Timer has elapsed, sound the buzzer
      digitalWrite(buzzerPin, HIGH );
      matrix.loadFrame(heart);// You can adjust the frequency as needed
      delay(10000);          // Sound for 1 second
      digitalWrite(buzzerPin, LOW);
      matrix.loadFrame(danger);// Stop the buzzer
      isCountingDown = false;
    }
  }
}

void displayTime(unsigned long time) {
  int minutes = time / 60000;
  int seconds = (time % 60000) / 1000;

  lc.setDigit(0, 7, minutes / 10, false);
  lc.setDigit(0, 6, minutes % 10, false);
  lc.setDigit(0, 5, seconds / 10, false);
  lc.setDigit(0, 4, seconds % 10, false);
  lc.setDigit(0, 3, minutes / 10, false);
  lc.setDigit(0, 2, minutes % 10, false);
  lc.setDigit(0, 1, seconds / 10, false);
  lc.setDigit(0, 0, seconds % 10, false);
  matrix.loadFrame(happy);
}
