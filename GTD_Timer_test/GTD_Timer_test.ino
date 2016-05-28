const int inPin0 = 2;
const int inPin1 = 3;

void setup(){
  Serial.begin(115200);          //  setup serial
  pinMode(inPin0, INPUT_PULLUP);
  pinMode(inPin1, INPUT_PULLUP);
}

void loop(){
  int reading0 = digitalRead(inPin0);
  int reading1 = digitalRead(inPin1);
  Serial.print(millis());
  Serial.print(",");
  Serial.print(reading0);
  Serial.print(",");
  Serial.println(reading1*1.2);
  delay(100);
}
