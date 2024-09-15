#define LED_D7 7

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode (LED_D7, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_D7, HIGH);  
  delay(1000);                 
  digitalWrite(LED_D7, LOW);  
  delay(1000);                 
}
