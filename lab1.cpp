#include "mbed.h"

DigitalOut redled(p5); //define and name a digital output on pin 5
DigitalOut yellowled(p13); //define and name a digital output on pin 13
DigitalOut greenled(p20); //define and name a digital output on pin 20
DigitalIn btn(p10); //define and name a digital input on pin 15

int main() {
    redled = 1;
    yellowled = 0;
    greenled = 0;
    while(1) {
        if (redled == 1 && btn) {
            yellowled = 1;
    	    ThisThread::sleep_for(2s);
    	    redled = 0;
    	    yellowled = 0;
    	    greenled = 1;
        }
        if (greenled == 1 && btn) {
            greenled = 0;
    	    yellowled = 1;
    	    ThisThread::sleep_for(2s);
    	    yellowled = 0;
    	    redled = 1;
        }
    ThisThread::sleep_for(1ms);
    }
}