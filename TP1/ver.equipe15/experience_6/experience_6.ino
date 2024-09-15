void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
}
void loop() {
  for (int i=0; i <= 255; i+=16) {
    analogWrite(3, i);
    for (int j=0; j <= 255; j+=16) {
      analogWrite(5, j);
      for (int k=0; k <= 255; k+=16) {
        analogWrite(6, k);
        delay(250);
      }
    }
  }
}