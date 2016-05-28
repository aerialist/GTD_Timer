/*
  GTD Timer

  Use 10 LED Bar to count two minutes.

 */

#include <avr/sleep.h>

// these constants won't change:
const int ledCount = 10;    // the number of LEDs in the bar graph
const int inPin0 = 2;         // the input pin for interrupt0
const int inPin1 = 3;         // the input pin for interrupt1

bool DEBUG = false;
int divider;
const int ledPins[] = {
  4, 5, 6, 7, 8, 9, 10, 11, 12, 13
};   // an array of pin numbers to which LEDs are attached
volatile bool checkOrientation = true;
volatile unsigned long firstMillis = 0;    // for debounce
volatile unsigned long previousMillis = 0;
volatile bool directionF = false;
bool previousDirection = false;
int reading0_1 = 0;  // temporary direction reading
int reading1_1 = 0;
int count = 0;         // interrupt counter

enum states {
  CHECK1, // check orientation for the 1st time
  CHECK2, // check orientation for the 2nd time
  RUNNING
};

volatile enum states state = CHECK1;

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
// PORTD: Digital Pin 7, 6, 5, 4, 3, 2, 1, 0
// PortB: Crystal, Crystal, Digital Pin 13, 12, 11, 10, 9, 8

uint8_t flip8(uint8_t v){
  // mirror uint8_t
  // http://forum.arduino.cc/index.php?topic=54304.15
  uint16_t s = sizeof(v) * 8;   // bit size; must be power of 2
  uint16_t mask = ~0;
  while ((s >>= 1) > 0)
  {
    mask ^= (mask << s);
    v = ((v >> s) & mask) | ((v << s) & ~mask);
  }
  return v;
}

void dynamicDrive(uint16_t pattern){
  // const uint16_t pattern100  = (B00000011 * 256) + B11111111;
  // const uint8_t pattern100b = B00111111, pattern100d = B11110000;
  if (directionF){
    // flip pattern for upside down
    uint8_t a = pattern >> 8;
    uint8_t b = pattern;
    pattern = (flip8(b) * 256 + flip8(a)) >> 6;
  }
  uint8_t patternb = pattern >> 4;
  uint8_t patternd = pattern << 4;
  for (int thisLed = 0; thisLed < ledCount; thisLed++){
    if (thisLed >= 0 && thisLed < 4){
      // PORTD
      bool myBit = bitRead(patternd, thisLed+4);
      uint8_t newPortD = PORTD & B00001111;
      PORTD = bitWrite(newPortD, thisLed+4, myBit);
      PORTB = PORTB & B11000000;
    }
    else {
      // PORTB
      bool myBit = bitRead(patternb, thisLed-4);
      uint8_t newPortB = PORTB & B11000000;
      PORTB = bitWrite(newPortB, thisLed-4, myBit);
      PORTD = PORTD & B00001111;
    }
    delay(1);
  }
}

void allOFF(){
  for (int thisLed = 0; thisLed < ledCount; thisLed++) {
    digitalWrite(ledPins[thisLed], LOW);
  }
}

void tilt_isr(){
  // isr during operation to detect flipping
  //detachInterrupt(digitalPinToInterrupt(inPin0));
  //Serial.print("Interrupt: ");
  //Serial.println(count++);
  state = CHECK1;
}

void sleep_isr(){
  // isr0 to wake up from sleep
  sleep_disable();
  detachInterrupt(0);
  detachInterrupt(1);
  state = CHECK1;
}

void go_sleep(){
  detachInterrupt(digitalPinToInterrupt(inPin0));
  detachInterrupt(digitalPinToInterrupt(inPin1));

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
    if (reading0) attachInterrupt(0, sleep_isr, LOW);
    if (reading1) attachInterrupt(1, sleep_isr, LOW);
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
  attachInterrupt(digitalPinToInterrupt(inPin0), tilt_isr, FALLING);
  attachInterrupt(digitalPinToInterrupt(inPin1), tilt_isr, FALLING);
}

void findOrientation1(){
  // make sure we read clean twice before and after debounce time
  //noInterrupts(); // disable interrupts i.e. cli();
  // 1st check
  Serial.print("Find orientation1: ");
  reading0_1 = digitalRead(inPin0);
  reading1_1 = digitalRead(inPin1);
  firstMillis = millis();
  state = CHECK2;
  Serial.println(firstMillis);
  //interrupts();
}
void findOrientation2(){
  // 2nd check
  //Serial.print("Find orientation2: ");
  unsigned long now = millis();
  //Serial.println(now);
  unsigned long delta = now - firstMillis;
  if (delta > 300){
    // waited for debounce time
    Serial.print("  Debounce time elapsed: ");
    int reading0 = digitalRead(inPin0);
    int reading1 = digitalRead(inPin1);
    if (reading0 == reading0_1 && reading1 == reading1_1) {
      // reading is stable
      if (reading0 + reading1 == 1){
        Serial.println("Good reading");
        // direction is definite
        Serial.print("    reading0: ");
        Serial.println(reading0);
        Serial.print("    reading1: ");
        Serial.println(reading1);
        Serial.print("    previousDirection: ");
        Serial.println(previousDirection);
        if (reading0) directionF = true;
        if (reading1) directionF = false;
        state = RUNNING;
        if (previousDirection != directionF) {
          Serial.println("    Reset direction");
          previousMillis = now;
          previousDirection = directionF;
        }
        else{
          Serial.println("    Continue");
        }
        return;
      }
    }
    // not clean. Do 1st again.
    Serial.println("Not good reading");
    findOrientation1();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting GTD timer!");
  // loop over the pin array and set them all to output:
  for (int thisLed = 0; thisLed < ledCount; thisLed++) {
    pinMode(ledPins[thisLed], OUTPUT);
  }
  allOFF();
  pinMode(inPin0, INPUT_PULLUP);
  pinMode(inPin1, INPUT_PULLUP);

  if (DEBUG) divider = 10;
  else divider = 1;

  attachInterrupt(digitalPinToInterrupt(inPin0), tilt_isr, FALLING);
  attachInterrupt(digitalPinToInterrupt(inPin1), tilt_isr, FALLING);
}

void loop() {
  unsigned long currentMillis = millis();
  unsigned long elapsedTime = currentMillis - previousMillis; // elapsed time since last isr

  if (state == CHECK1){
    findOrientation1();
  } else if (state == CHECK2) {
    findOrientation2();
  } else{
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
      //Serial.print(elapsedTime);
      //Serial.println(": >120");
      // alternate by a second
      if ((elapsedTime / 1000) % 2){
        dynamicDrive(pattern0);
      } else {
        dynamicDrive(pattern100);
      }
    } else if (elapsedTime > 110000 / divider){
      //Serial.print(elapsedTime);
      //Serial.println(": >110");
      // alternate by a 100 ms
      if ((elapsedTime / 100) % 2){
        dynamicDrive(pattern90);
      } else {
        dynamicDrive(pattern100);
      }
    } else if (elapsedTime > 100000 / divider){
      //Serial.print(elapsedTime);
      //Serial.println(": >100");
      // alternate by a second
      if ((elapsedTime / 1000) % 2){
        dynamicDrive(pattern90);
      } else {
        dynamicDrive(pattern100);
      }
    } else if (elapsedTime > 90000 / divider){
      //Serial.print(elapsedTime);
      //Serial.println(": >90");
      dynamicDrive(pattern90);
    } else if (elapsedTime > 80000 / divider){
      //Serial.print(elapsedTime);
      //Serial.println(": >80");
      dynamicDrive(pattern80);
    } else if (elapsedTime > 70000 / divider){
      //Serial.print(elapsedTime);
      //Serial.println(": >70");
      dynamicDrive(pattern70);
    } else if (elapsedTime > 60000 / divider){
      //Serial.print(elapsedTime);
      //Serial.println(": >60");
      dynamicDrive(pattern60);
    } else if (elapsedTime > 50000 / divider){
      //Serial.print(elapsedTime);
      //Serial.println(": >50");
      dynamicDrive(pattern50);
    } else if (elapsedTime > 40000){
      //Serial.print(elapsedTime);
      //Serial.println(": >40");
      dynamicDrive(pattern40);
    } else if (elapsedTime > 30000){
      //Serial.print(elapsedTime);
      //Serial.println(": >30");
      dynamicDrive(pattern30);
    } else if (elapsedTime > 20000){
      //Serial.print(elapsedTime);
      //Serial.println(": >20");
      dynamicDrive(pattern20);
    } else if (elapsedTime > 10000){
      //Serial.print(elapsedTime);
      //Serial.println(": >10");
      dynamicDrive(pattern10);
    } else {
      // less than 10 sec
      //Serial.print(elapsedTime);
      //Serial.println(": Less than 10");
      // alternate by a second
      if ((elapsedTime / 1000) % 2){
        dynamicDrive(pattern0);
      } else {
        dynamicDrive(pattern10);
      }
    }
  }
}
