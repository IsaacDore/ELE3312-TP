void setup() {
  pinMode(6, OUTPUT);
}

void loop() {
  analogWrite(6, 255); delay(1000);
  analogWrite(6, 127); delay(2000);
  analogWrite(6, 0); delay(3000);
}
