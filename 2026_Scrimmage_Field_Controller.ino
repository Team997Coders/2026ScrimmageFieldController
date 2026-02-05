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

//----------STATE-VARIABLES----------//
enum SequenceState { IDLE, RUNNING };
SequenceState sequenceState = IDLE;

unsigned long sequenceStartTime = 0;
unsigned long stageStartTime = 0;
int currentStage = 0;  // 0: auto, 1: transition, 2-9: teleop, 10: endgame
bool redFirst = false;

unsigned long lastButtonPressTime = 0;
const unsigned long DEBOUNCE_DELAY = 200;  // milliseconds


void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(START_BUTTON_PIN, INPUT_PULLUP);
  
  disableAlliances();
}


void loop() {
  // Check button with debouncing
  checkButton();
  
  // Run sequence if it's active
  if (sequenceState == RUNNING) {
    updateSequence();
  }
}


//----------SEQUENCES----------//

// Handle button press with debouncing
void checkButton() {
  if (digitalRead(START_BUTTON_PIN) == LOW) {
    unsigned long currentTime = millis();
    
    // Debounce: only register press if enough time has passed since last press
    if (currentTime - lastButtonPressTime >= DEBOUNCE_DELAY) {
      lastButtonPressTime = currentTime;
      
      if (sequenceState == IDLE) {
        // Start the sequence
        sequenceState = RUNNING;
        currentStage = 0;
        redFirst = random(0, 2);  // Fixed: random can return 0 or 1
        sequenceStartTime = millis();
        stageStartTime = millis();
      } else {
        // Cancel/reset the sequence
        sequenceState = IDLE;
        currentStage = 0;
        disableAlliances();
      }
    }
  }
}

// Non-blocking sequence update
void updateSequence() {
  unsigned long elapsedTime = millis() - stageStartTime;
  
  switch (currentStage) {
    case 0:  // AUTO - 20 seconds
      enableAlliances();
      if (elapsedTime >= 20000) {
        currentStage = 1;
        stageStartTime = millis();
      }
      break;
      
    case 1:  // TRANSITION - 10 seconds
      disableAlliances();
      if (elapsedTime >= 10000) {
        currentStage = 2;
        stageStartTime = millis();
      }
      break;
      
    case 2:  // TELEOP 1
    case 4:  // TELEOP 3
      if (redFirst == (currentStage == 2)) {
        redAlliance();
      } else {
        blueAlliance();
      }
      if (elapsedTime >= 25000) {
        currentStage++;
        stageStartTime = millis();
      }
      break;
      
    case 3:  // TELEOP 2
    case 5:  // TELEOP 4
      if (redFirst == (currentStage == 2)) {
        blueAlliance();
      } else {
        redAlliance();
      }
      if (elapsedTime >= 25000) {
        currentStage++;
        stageStartTime = millis();
      }
      break;
      
    case 6:  // ENDGAME - 30 seconds
      enableAlliances();
      if (elapsedTime >= 30000) {
        // Sequence complete
        sequenceState = IDLE;
        currentStage = 0;
        disableAlliances();
      }
      break;
  }
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



//----------TESTING----------//
void test() {
  digitalWrite(RED_PIN, 0);
  digitalWrite(BLUE_PIN, 1);

  delay(1000);

  digitalWrite(RED_PIN, 1);
  digitalWrite(BLUE_PIN, 0);

  delay(1000);
}