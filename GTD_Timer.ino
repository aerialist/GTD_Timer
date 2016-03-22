/*
  GTD Timer

  Use 10 LED Bar to count two minutes.

 */

#include <avr/sleep.h>

// these constants won't change:
const int ledCount = 10;    // the number of LEDs in the bar graph
const int buttonPin = A5;   // the pin that the button is attached to
const int inPin0 = 2;         // the input pin for interrupt0
const int inPin1 = 3;

bool DEBUG = true;
int divider;

const int ledPins[] = {
  4, 5, 6, 7, 8, 9, 10, 11, 12, 13
};   // an array of pin numbers to which LEDs are attached
volatile unsigned long previousMillis = 0;
volatile bool directionF = false;
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin
int count = 0;         // interrupt counter
volatile bool running = false;

const uint16_t pattern0    = (B00000000 * 256) + B00000000; // Binary formatter only works for 8 bit values
const uint16_t pattern10   = (B00000000 * 256) + B00000001;
const uint16_t pattern20   = (B00000000 * 256) + B00000011;
const uint16_t pattern30   = (B00000000 * 256) + B00000111;
const uint16_t pattern40   = (B00000000 * 256) + B00001111;
const uint16_t pattern50   = (B00000000 * 256) + B00011111;
const uint16_t pattern60   = (B00000000 * 256) + B00111111;
const uint16_t pattern70   = (B00000000 * 256) + B01111111;
const uint16_t pattern80   = (B00000000 * 256) + B11111111;
const uint16_t pattern90   = (B00000001 * 256) + B11111111;
const uint16_t pattern100  = (B00000011 * 256) + B11111111;

void dynamicDrive(uint16_t pattern){
  // TODO: Use Timer interrupt rather than delay
  // TODO: Use AVR BIT set code
  // http://playground.arduino.cc/Main/AVR
  // TODO: Set Port register directly
  for (int thisLed = 0; thisLed < ledCount; thisLed++){
    if (pattern & (1<<thisLed)){  // test pattern's thisLed-th bit
      if (directionF) digitalWrite(ledPins[thisLed], HIGH);
      else digitalWrite(ledPins[ledCount-thisLed-1], HIGH);
      delay(1); // 3ms makes flicker, 0ms dims for 16MHz
      if (directionF) digitalWrite(ledPins[thisLed], LOW);
      else digitalWrite(ledPins[ledCount-thisLed-1], LOW);
    }
  }
}

void allOFF(){
  for (int thisLed = 0; thisLed < ledCount; thisLed++) {
    digitalWrite(ledPins[thisLed], LOW);
  }
}

void tilt0_isr(){
  sleep_disable();
  detachInterrupt(0);
  Serial.print("Interrupt0: ");
  Serial.println(count++);
  running = true;
  previousMillis = millis();
  directionF = true;
}

void tilt1_isr(){
  sleep_disable();
  detachInterrupt(1);
  Serial.print("Interrupt1: ");
  Serial.println(count++);
  running = true;
  previousMillis = millis();
  directionF = false;
}

void go_sleep(){
  int reading0 = digitalRead(inPin0);
  int reading1 = digitalRead(inPin1);
  Serial.print("Pin0: ");
  Serial.println(reading0);
  Serial.print("Pin1: ");
  Serial.println(reading1);
  delay(100);
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  noInterrupts(); // disable interrupts i.e. cli();
  if (reading0 != reading1){
    Serial.println("Going to sleep!");
    sleep_enable(); // Set the SE (sleep enable) bit.
    if (reading0) attachInterrupt(0, tilt0_isr, LOW);
    if (reading1) attachInterrupt(1, tilt1_isr, LOW);
    //sleep_bod_disable();
    interrupts(); // enable interrupts i.e. sei();
    sleep_cpu();  // Put the device into sleep mode. The SE bit must be set beforehand, and it is recommended to clear it afterwards.
    
    /* wake up here */
    sleep_disable();
    Serial.println("I'm awake!");
  } else {
    Serial.println("I'm not sleepy");
  }
  interrupts(); // end of some_condition
}

void setup() {
  Serial.begin(57600);
  // loop over the pin array and set them all to output:
  for (int thisLed = 0; thisLed < ledCount; thisLed++) {
    pinMode(ledPins[thisLed], OUTPUT);
  }
  allOFF();
  pinMode(buttonPin, INPUT);
  pinMode(inPin0, INPUT_PULLUP);
  pinMode(inPin1, INPUT_PULLUP);
  
  if (DEBUG) divider = 10;
  else divider = 1;
}

void loop() {
  unsigned long currentMillis = millis();
  if (digitalRead(buttonPin)){
    // button is pressed!
    previousMillis = currentMillis;
    directionF = !directionF;
  }
  unsigned long elapsedTime = currentMillis - previousMillis;
  
  if (elapsedTime > 180000 / divider){
    // three minutes has passed.
    // go to sleep
    Serial.print(elapsedTime);
    Serial.println(": >180 Going to sleep");
    allOFF();
    go_sleep();
  } else if (elapsedTime > 120000 / divider){
    // two minutes has passed.
    // run final dance
    Serial.print(elapsedTime);
    Serial.println(": >120");
    // alternate by a second
    if ((elapsedTime / 1000) % 2){
      dynamicDrive(pattern0);
    } else {
      dynamicDrive(pattern100);
    }
  } else if (elapsedTime > 110000 / divider){
    Serial.print(elapsedTime);
    Serial.println(": >110");
    // alternate by a 100 ms
    if ((elapsedTime / 100) % 2){
      dynamicDrive(pattern90);
    } else {
      dynamicDrive(pattern100);
    }
  } else if (elapsedTime > 100000 / divider){
    Serial.print(elapsedTime);
    Serial.println(": >100");
    // alternate by a second
    if ((elapsedTime / 1000) % 2){
      dynamicDrive(pattern90);
    } else {
      dynamicDrive(pattern100);
    }
  } else if (elapsedTime > 90000 / divider){
    Serial.print(elapsedTime);
    Serial.println(": >90");
    dynamicDrive(pattern90);
  } else if (elapsedTime > 80000 / divider){
    Serial.print(elapsedTime);
    Serial.println(": >80");
    dynamicDrive(pattern80);
  } else if (elapsedTime > 70000 / divider){
    Serial.print(elapsedTime);
    Serial.println(": >70");
    dynamicDrive(pattern70);
  } else if (elapsedTime > 60000 / divider){
    Serial.print(elapsedTime);
    Serial.println(": >60");
    dynamicDrive(pattern60);
  } else if (elapsedTime > 50000 / divider){
    Serial.print(elapsedTime);
    Serial.println(": >50");
    dynamicDrive(pattern50);
  } else if (elapsedTime > 40000){
    Serial.print(elapsedTime);
    Serial.println(": >40");
    dynamicDrive(pattern40);
  } else if (elapsedTime > 30000){
    Serial.print(elapsedTime);
    Serial.println(": >30");
    dynamicDrive(pattern30);
  } else if (elapsedTime > 20000){
    Serial.print(elapsedTime);
    Serial.println(": >20");
    dynamicDrive(pattern20);
  } else if (elapsedTime > 10000){
    Serial.print(elapsedTime);
    Serial.println(": >10");
    dynamicDrive(pattern10);
  } else {
    // less than 10 sec
    Serial.print(elapsedTime);
    Serial.println(": Less than 10");
    // alternate by a second
    if ((elapsedTime / 1000) % 2){
      dynamicDrive(pattern0);
    } else {
      dynamicDrive(pattern10);
    }
  }
}



