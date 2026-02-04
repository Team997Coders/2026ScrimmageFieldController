#define RED_PIN 2 //pin for red alliance
#define BLUE_PIN 3 //pin for blue alliance

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
}

void loop() {
  digitalWrite(RED_PIN, 0);
  digitalWrite(BLUE_PIN, 1);

  delay(1000);

  digitalWrite(RED_PIN, 1);
  digitalWrite(BLUE_PIN, 0);

  delay(1000);
}
