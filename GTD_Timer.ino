/*
  GTD Timer

  Use 10 LED Bar to count two minutes.

 */


// these constants won't change:
const int ledCount = 10;    // the number of LEDs in the bar graph
const int buttonPin = 13;   // the pin that the button is attached to

int ledPins[] = {
  2, 3, 4, 5, 6, 7, 8, 9, 10, 11
};   // an array of pin numbers to which LEDs are attached
unsigned long previousMillis = 0;

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

void dynamicDrive(uint16_t pattern){
  for (int thisLed = 0; thisLed < ledCount; thisLed++){
    if (pattern & (1<<thisLed)){  // test pattern's thisLed-th bit
      digitalWrite(ledPins[thisLed], HIGH);
      delay(1); // 3ms makes flicker, 0ms dims for 16MHz
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
  Serial.begin(57600);
  // loop over the pin array and set them all to output:
  for (int thisLed = 0; thisLed < ledCount; thisLed++) {
    pinMode(ledPins[thisLed], OUTPUT);
  }
  allOFF();
  pinMode(buttonPin, INPUT);
}

void loop() {
  unsigned long currentMillis = millis();
  if (digitalRead(buttonPin)){
    // button is pressed!
    previousMillis = currentMillis;
  }
  unsigned long elapsedTime = currentMillis - previousMillis;
  
  if (elapsedTime > 180000){
    // three minutes has passed.
    // go to sleep
    Serial.print(elapsedTime);
    Serial.println(": >180 Going to sleep");
    allOFF();
  } else if (elapsedTime > 120000){
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
  } else if (elapsedTime > 110000){
    Serial.print(elapsedTime);
    Serial.println(": >110");
    // alternate by a 100 ms
    if ((elapsedTime / 100) % 2){
      dynamicDrive(pattern90);
    } else {
      dynamicDrive(pattern100);
    }
  } else if (elapsedTime > 100000){
    Serial.print(elapsedTime);
    Serial.println(": >100");
    // alternate by a second
    if ((elapsedTime / 1000) % 2){
      dynamicDrive(pattern90);
    } else {
      dynamicDrive(pattern100);
    }
  } else if (elapsedTime > 90000){
    Serial.print(elapsedTime);
    Serial.println(": >90");
    dynamicDrive(pattern90);
  } else if (elapsedTime > 80000){
    Serial.print(elapsedTime);
    Serial.println(": >80");
    dynamicDrive(pattern80);
  } else if (elapsedTime > 70000){
    Serial.print(elapsedTime);
    Serial.println(": >70");
    dynamicDrive(pattern70);
  } else if (elapsedTime > 60000){
    Serial.print(elapsedTime);
    Serial.println(": >60");
    dynamicDrive(pattern60);
  } else if (elapsedTime > 50000){
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



