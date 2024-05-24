#include "mbed.h"
BufferedSerial async_port(p9, p10); //set up TX and RX on pins 9 and 10

DigitalOut redled(p5); //define and name a digital output on pin 5
DigitalOut yellowled(p13); //define and name a digital output on pin 13
DigitalOut greenled(p20); //define and name a digital output on pin 20

DigitalIn btn(p21); //define and name a digital input on pin 15

char buttonPressed;
char buffer[1] = {};

int main() {
    
    async_port.set_format(8, BufferedSerial::None, 1);
    async_port.set_baud(9600);
    
    redled = 1;
    yellowled = 0;
    greenled = 0;
    
    while(1) {
        if (async_port.readable() == 1){
            int result = async_port.read(buffer, 1); 

            if (result <= 0){
                continue;
            }

            buttonPressed = buffer[0];
        
            if (redled == 1 && buttonPressed) {
                yellowled = 1;
                ThisThread::sleep_for(2s);
                redled = 0;
                yellowled = 0;
                greenled = 1;
                buffer[0] = buttonPressed = 0;
                ThisThread::sleep_for(500ms);
            }
            if (greenled == 1 && buttonPressed) {
                greenled = 0;
                yellowled = 1;
                ThisThread::sleep_for(2s);
                yellowled = 0;
                redled = 1;
                buffer[0] = buttonPressed = 0;
            }
        }
        else if (async_port.writable()){
            if (btn){
                buffer[0] = 1;
                async_port.write(buffer, 1);
                ThisThread::sleep_for(500ms);
            }

        }

    ThisThread::sleep_for(1ms);
    }
}
