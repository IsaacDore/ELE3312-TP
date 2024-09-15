/*
  laboratoire = 1
  groupe = 2
  date = 2024/septembre/6
  auteur = Isaac Doré
  équipe = 15
*/

const int GREEN_1 = 2;
const int GREEN_2 = 3;
const int YELLOW_1 = 4;
const int YELLOW_2 = 5;
const int RED_1 = 6;
const int RED_2 = 7;

int input_signal = 0;
const int LED_CNT = 6;
const int LED[LED_CNT] = { GREEN_1, GREEN_2, YELLOW_1, YELLOW_2, RED_1, RED_2 };
const int PLAGE[LED_CNT] = {200, 400, 550, 700, 800, 900}; //1024 implied

void setup() {
  for(int i=0; i<LED_CNT; i++){
    pinMode(LED[i], OUTPUT);
  }
}

void loop() {
  input_signal = analogRead(0);
  for(int i=0; i<LED_CNT; i++){
    digitalWrite(LED[i], input_signal < PLAGE[i] ? LOW : HIGH);
  }
}
