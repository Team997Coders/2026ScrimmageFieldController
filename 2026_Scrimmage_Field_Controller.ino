#include <Arduino.h>

//----------MATCH-SCHEDULE-REFERENCE----------//
//AUTO: both alliances enabled for 20 seconds
//TRANSITION: both alliances disabled for 10 seconds
//TELEOP: each alliance enabled for 25 seconds, alternating, total of 4 cycles
  //(RED first or BLUE first is random for the purposes of scrimmage, though determined by the higher score in auto in an official match)
//ENDGAME: both alliances enabled for 30 seconds
//-------------------------------------------//


#define RED_PIN 2 //pin for red alliance
#define BLUE_PIN 3 //pin for blue alliance

#define START_BUTTON_PIN 4 //pin for match start button


void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(START_BUTTON_PIN, INPUT_PULLUP);

  delaySeconds(3);
  matchSequence();
}


void loop() {
  delaySeconds(1);
}


//----------SEQUENCES----------//
void matchSequence() {
  //auto
  enableAlliances();
  delaySeconds(20);

  //transition
  delaySeconds(10);

  //teleop
  bool redFirst = random(0, 1);
  for (int i = 0; i < 4; i++) {
    if (redFirst) {
      redAlliance();
    } else {
      blueAlliance();
    }

    delaySeconds(25);

    redFirst = !redFirst;
  }

  //endgame
  enableAlliances();
  delaySeconds(30);
}


//----------FUNCTION-DECLARATIONS----------//
//enable red alliance side
void redAlliance() {
  digitalWrite(RED_PIN, 1);
  digitalWrite(BLUE_PIN, 0);
}
//enable blue alliance side
void blueAlliance() {
  digitalWrite(RED_PIN, 0);
  digitalWrite(BLUE_PIN, 1);
}
//disable both sides
void disableAlliances() {
  digitalWrite(RED_PIN, 0);
  digitalWrite(BLUE_PIN, 0);
}
//enable both sides
void enableAlliances() {
  digitalWrite(RED_PIN, 1);
  digitalWrite(BLUE_PIN, 1);
}

//delay in seconds
void delaySeconds(int seconds) {
  delay(seconds * 1000);
}



//----------TESTING----------//
void test() {
  digitalWrite(RED_PIN, 0);
  digitalWrite(BLUE_PIN, 1);

  delay(1000);

  digitalWrite(RED_PIN, 1);
  digitalWrite(BLUE_PIN, 0);

  delay(1000);
}