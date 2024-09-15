void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(9600);
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
}
void loop() {
  for (int i=0; i <= 255; i+=16) {
    Serial.print("\nOn change la couleur de D3 : ");
    Serial.print(i);
    analogWrite(3, i);
    for (int j=0; j <= 255; j+=16) {
      Serial.print("\nOn change la couleur de D5 : ");
      Serial.print(j);
      analogWrite(5, j);
      for (int k=0; k <= 255; k+=16) {
        Serial.print(".");
        analogWrite(6, k);
        delay(50);
      }
    }
  }
}
