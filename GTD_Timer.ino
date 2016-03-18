/*
  LED bar graph

  Turns on a series of LEDs based on the value of an analog sensor.
  This is a simple way to make a bar graph display. Though this graph
  uses 10 LEDs, you can use any number by changing the LED count
  and the pins in the array.

  This method can be used to control any series of digital outputs that
  depends on an analog input.

  The circuit:
   * LEDs from pins 2 through 11 to ground

 created 4 Sep 2010
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/BarGraph
 */


// these constants won't change:
const int analogPin = A0;   // the pin that the potentiometer is attached to
const int ledCount = 10;    // the number of LEDs in the bar graph
const int buttonPin = 13;   // the pin that the button is attached to

int ledPins[] = {
  2, 3, 4, 5, 6, 7, 8, 9, 10, 11
};   // an array of pin numbers to which LEDs are attached

uint16_t pattern0   = (B00000000 * 256) + B00000000; // Binary formatter only works for 8 bit values
uint16_t pattern10  = (B00000000 * 256) + B00000001;
uint16_t pattern20  = (B00000000 * 256) + B00000011;
uint16_t pattern30  = (B00000000 * 256) + B00000111;
uint16_t pattern40  = (B00000000 * 256) + B00001111;
uint16_t pattern50  = (B00000000 * 256) + B00011111;
uint16_t pattern60  = (B00000000 * 256) + B00111111;
uint16_t pattern70  = (B00000000 * 256) + B01111111;
uint16_t pattern80  = (B00000000 * 256) + B11111111;
uint16_t pattern90  = (B00000001 * 256) + B11111111;
uint16_t pattern100 = (B00000011 * 256) + B11111111;
uint16_t pattern110 = (B00000011 * 256) + B11111111;

void dynamicDrive(uint16_t pattern){
  for (int thisLed = 0; thisLed < ledCount; thisLed++){
    if (pattern & (1<<thisLed)){  // test pattern's thisLed-th bit
      digitalWrite(ledPins[thisLed], HIGH);
      delay(1); // 3ms makes flicker, 0ms dims
      digitalWrite(ledPins[thisLed], LOW);
    }
  }
}

void allOFF(){
  for (int thisLed = 0; thisLed < ledCount; thisLed++) {
    digitalWrite(ledPins[thisLed], LOW);
  }
}

void setup() {
  // loop over the pin array and set them all to output:
  for (int thisLed = 0; thisLed < ledCount; thisLed++) {
    pinMode(ledPins[thisLed], OUTPUT);
  }
  allOFF();
  pinMode(buttonPin, INPUT);
}

void loop() {
  dynamicDrive(pattern90);
}



