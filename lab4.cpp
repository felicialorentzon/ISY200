#include "C12832/C12832.h"
#include "mbed.h"
#include <string>

using namespace std;

#define MAX 300
#define MIN 20
#define STEP 4
#define TONE 1.0 / 440
#define INTENSITY 0.5
#define BAUD 9600
#define PERIOD 1000
#define DEBOUNCE_DURATION 200

void beat();
void raise_beat();
void lower_beat();
void update_lcd();
void write_rate(int rate);
void print(std::string input);

Serial pc(USBTX, USBRX, BAUD);
InterruptIn up_button(p15);
InterruptIn down_button(p12);
DigitalOut redled(p23);
DigitalOut blueled(p25);
DigitalOut greenled(p24);
Ticker beat_rate;
Timer debouncer;
C12832 lcd(p5, p7, p6, p8, p11);
PwmOut speaker(p26);
float period;
int rate(120);

int main() {
  up_button.rise(&raise_beat);
  down_button.rise(&lower_beat);
  greenled = 1;
  blueled = 1;
  redled = 0;
  speaker.period(TONE);
  speaker = 0.5;
  debouncer.start();
  period = 60.0 / (rate*2);

  string screenTextDefault = "\r\nmbed metronome!\r\n_______________\r\n";
  print(screenTextDefault);

  beat_rate.attach(&beat, period);

  lcd.cls();
  lcd.locate(0, 0);
  update_lcd();

  while (1) {
    update_lcd();
    ThisThread::sleep_for(500);
  }
}

void beat() {
  beat_rate.attach(&beat, period);
  redled = !redled;
  
  if (speaker){
    speaker = 0;
  }
  else {
    speaker = 0.5;
  } 
}

void raise_beat() {
  if (debouncer.read_ms() > DEBOUNCE_DURATION) {
    debouncer.reset();
    rate += STEP;

    if (rate > MAX)
      rate = MAX;

    period = 60.0 / (rate*2);
  }
}

void lower_beat() {
  
  if (debouncer.read_ms() > DEBOUNCE_DURATION) {
    debouncer.reset();
    rate -= STEP;

    if (rate < MIN)
      rate = MIN;

    period = 60.0 / (rate*2);}
}

void update_lcd() {
  lcd.locate(0, 2);
  lcd.printf("Felicia o Stina");
  lcd.printf("%d bpm", rate);

}

void write_rate(int rate) {
  std::string rate_str = std::to_string(rate);
  print("metronome rate is " + rate_str + "\n");
}

void print(std::string input) { pc.write(input.c_str(), input.length()); }