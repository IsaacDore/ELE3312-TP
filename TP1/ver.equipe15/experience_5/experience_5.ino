int x = 0;
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(6, OUTPUT);
}
void loop() {
  x = analogRead(0);
  analogWrite(6, x/4);
}
