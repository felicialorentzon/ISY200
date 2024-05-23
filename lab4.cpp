#include "C12832.h"
#include "debug.h"
#include "mbed.h"
#include <chrono>
#include <stdio.h>

#define MAX 300
#define MIN 20
#define STEP 4
#define TONE 1.0 / 440
#define INTENSITY 0.5
#define DURATION 100ms

BufferedSerial pc(USBTX, USBRX);
InterruptIn up_button(p15);
InterruptIn down_button(p12);
DigitalOut redled(p23); // displays the metronome beat
DigitalOut blueled(p25);
DigitalOut greenled(p24);
Ticker beat_rate; // define a Ticker, with name “beat_rate”
Ticker up_debouncer;
Ticker down_debouncer;
C12832 lcd(p5, p7, p6, p8, p11);
PwmOut spkr(p26);

void beat();
void raise_beat();
void lower_beat();
void enable_up_interrupt();
void enable_down_interrupt();

// metronome period in seconds, inital value 0.5
std::chrono::milliseconds period = 500ms;

int rate(120); // metronome rate, initial value 120

int main() {

  up_button.rise(&raise_beat);
  down_button.rise(&lower_beat);
  greenled = 1;
  blueled = 1;
  redled = 0;
  spkr.period(TONE);
  spkr = INTENSITY;

  pc.write("\r\n", 2);
  pc.write("mbed metronome!\r\n", 17);
  pc.write("_______________\r\n", 17);

  period = 1;
  redled = 1; // diagnostic

  ThisThread::sleep_for(100ms);

  beat_rate.attach(&beat, period); // initialises the beat rate

  // main loop checks buttons, updates rates and displays
  int j = 0;
  lcd.cls();
  lcd.locate(0, 3);
  lcd.printf("Metronome");

  while (1) {

    period = 60 / rate; // calculate the beat period

    pc.write("metronome rate is %i\r", rate);
    // pc.printf("metronome period is %f\r\n", period); //optional check

    lcd.locate(0, 15);
    lcd.printf("%d bpm", j);
    j++;

    ThisThread::sleep_for(500ms);
  }
}

void beat() { // this is the metronome beat
  beat_rate.attach(&beat, period);
  redled = !redled;

  if (spkr) {
    spkr = 0;

  } else {
    spkr = INTENSITY;
  }
}

void enable_up_interrupt(){
  up_debouncer.detach();
  up_button.enable_irq();
}

void enable_down_interrupt(){
  down_debouncer.detach();
  down_button.enable_irq();
}

void raise_beat() {
  rate += STEP;

  if (rate > MAX)
    rate = MAX;
  
  up_button.disable_irq();
  up_debouncer.attach(&enable_up_interrupt, DURATION);
}

void lower_beat() {
  rate -= STEP;

  if (rate < MIN)
    rate = MIN;

  down_button.disable_irq();
  down_debouncer.attach(&enable_down_interrupt, DURATION);
}