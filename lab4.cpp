#include "C12832/C12832.h"
#include "mbed.h"
#include <chrono>
#include <string>

using namespace std;

#define MAX 300
#define MIN 20
#define STEP 4
#define TONE 1.0 / 440
#define INTENSITY 0.5
#define DURATION 100ms
#define BAUD 9600
#define PERIOD 500ms
#define DEBOUNCE_DURATION 200ms

void beat();
void raise_beat();
void lower_beat();
void update_lcd();
void write_rate(int rate);
void print(std::string input);

BufferedSerial pc(USBTX, USBRX, BAUD);
InterruptIn up_button(p15);
InterruptIn down_button(p12);
DigitalOut redled(p23);
DigitalOut blueled(p25);
DigitalOut greenled(p24);
Ticker beat_rate;
Timer debouncer;
C12832 lcd(p5, p7, p6, p8, p11);
PwmOut speaker(p26);
std::chrono::milliseconds period = 500ms;
int rate(120);

int main() {
  up_button.rise(&raise_beat);
  down_button.rise(&lower_beat);
  greenled = 1;
  blueled = 1;
  redled = 0;
  speaker.period(TONE);
  speaker = INTENSITY;
  debouncer.start();

  print("\r\n");
  print("mbed metronome!\r\n");
  print("_______________\r\n");

  period = PERIOD;
  redled = 1;

  beat_rate.attach(&beat, period);

  lcd.cls();
  lcd.locate(0, 3);
  lcd.printf("Metronome");

  while (1) {
    ThisThread::sleep_for(100ms);
  }
}

void beat() {
  beat_rate.attach(&beat, period);
  redled = !redled;

  if (speaker) {
    speaker = 0;
  } else {
    speaker = INTENSITY;
  }
}

void raise_beat() {
  if (std::chrono::duration_cast<std::chrono::milliseconds>(
          debouncer.elapsed_time()) > DEBOUNCE_DURATION) {
    debouncer.reset();
    rate += STEP;

    if (rate > MAX)
      rate = MAX;

    period = 60s / rate;
    write_rate(rate);
  }
}

void lower_beat() {
  if (std::chrono::duration_cast<std::chrono::milliseconds>(
          debouncer.elapsed_time()) > DEBOUNCE_DURATION) {
    debouncer.reset();
    rate -= STEP;

    if (rate < MIN)
      rate = MIN;

    period = 60s / rate;
    write_rate(rate);
  }
}
void update_lcd() {
  lcd.locate(0, 15);
  lcd.printf("%d bpm", rate);
}

void write_rate(int rate) {
  std::string rate_str = std::to_string(rate);
  print("metronome rate is " + rate_str + "\n");
}

void print(std::string input) { pc.write(input.c_str(), input.length()); }