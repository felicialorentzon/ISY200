#include "mbed.h"
#include <chrono>

#define SERVO_MIN 900
#define SERVO_MAX 2100
#define SERVO_PERIOD 20
#define SERVO_SCALE 1
#define LED_PERIOD 0.001
#define LED_SCALE 0.1
#define MIN_SWEEP_TIME 1000
#define MAX_SWEEP_TIME 5000
#define COLOR_MAX 255
#define RED_MAX 0xC6
#define GREEN_MAX 0x73
#define BLUE_MAX 0xFF

// Define pins for peripherals
PwmOut servo(p21);

PwmOut redLED(p23);   // Red RGB LED
PwmOut greenLED(p24); // Green RGB LED
PwmOut blueLED(p25);  // Blue RGB LED

AnalogIn potentiometer(p20);
DigitalIn joystickUp(p15);
DigitalIn joystickDown(p12);

enum Direction { Left, Right };

Direction direction = Direction::Right;
int counter = 0;
bool wiperActive = false;
int sweepTime = MIN_SWEEP_TIME;

void setColour(int r, int g, int b) {
  redLED = (1 - (float)r / COLOR_MAX) * LED_SCALE;
  greenLED = (1 - (float)g / COLOR_MAX) * LED_SCALE;
  blueLED = (1 - (float)b / COLOR_MAX) * LED_SCALE;
}

void setup() {
  // Standard servo period
  servo.period_ms(SERVO_PERIOD);

  // Servo starts at the far right position
  servo.pulsewidth_us(SERVO_MAX);

  // High frequency to control brightness via duty cycle
  redLED.period(LED_PERIOD);
  greenLED.period(LED_PERIOD);
  blueLED.period(LED_PERIOD);

  // Start with LED off
  setColour(0, 0, 0);
}

int main() {
  setup();

  while (true) {
    // Convert range [0.0, 1.0] to [MIN_SWEEP_TIME, MAX_SWEEP_TIME] milliseconds
    sweepTime =
        MIN_SWEEP_TIME + (MAX_SWEEP_TIME - MIN_SWEEP_TIME) * potentiometer;

    if (wiperActive) {
      switch (direction) {
      case Direction::Left:
        if (counter == SERVO_SCALE * sweepTime) {
          setColour(RED_MAX, GREEN_MAX, BLUE_MAX);
          direction = Direction::Right;
        }
        break;
      case Direction::Right:
        if (counter == SERVO_SCALE * sweepTime) {
          setColour(0, 0, 0);
          direction = Direction::Left;
        }
        break;
      }
    } else {
      setColour(0, 0, 0);
      direction = Direction::Right;
    }

    if (joystickUp) {
      wiperActive = true;
    }
    if (joystickDown) {
      wiperActive = false;
    }

    counter += direction == Direction::Left ? -1 : 1;

    int stepSize = (SERVO_MAX - SERVO_MIN) / (SERVO_SCALE * sweepTime);
    int pulseWidth = SERVO_MIN + stepSize * counter;
    if (pulseWidth < SERVO_MIN) {
      pulseWidth = SERVO_MIN;
      counter = 0;
    } else if (pulseWidth > SERVO_MAX) {
      pulseWidth = SERVO_MAX;
      counter = SERVO_SCALE * sweepTime;
    }

    servo.pulsewidth_us(pulseWidth);

    ThisThread::sleep_for(1ms);
  }
}