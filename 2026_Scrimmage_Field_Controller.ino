#include <Arduino.h>

// ---------- PIN DEFINITIONS ----------
#define RED_PIN          2
#define BLUE_PIN         3
#define START_BUTTON_PIN 4

// ---------- TIMING CONSTANTS ----------
const unsigned long AUTO_TIME       = 20000;
const unsigned long TRANSITION_TIME = 10000;
const unsigned long TELEOP_TIME     = 25000;
const unsigned long ENDGAME_TIME    = 30000;
const unsigned long DEBOUNCE_DELAY  = 500;

const unsigned long WARNING_TIME    = 3000;   // last 3 seconds
const unsigned long BLINK_INTERVAL  = 500;    // blink rate

// ---------- MATCH STATE ----------
enum MatchState {
  IDLE,
  AUTO,
  TRANSITION,
  TELEOP,
  ENDGAME
};

MatchState matchState = IDLE;

// ---------- STATE VARIABLES ----------
unsigned long stageStartTime = 0;
unsigned long lastButtonTime = 0;

bool redFirst = false;
bool redTurn  = false;
int  teleopCycle = 0;

// ---------- FUNCTION DECLARATIONS ----------
void startMatch();
void resetMatch();
void updateMatch();

void redAlliance();
void blueAlliance();
void enableAlliances();
void disableAlliances();

void blinkRed();
void blinkBlue();
void digitalWriteInverted(int pin, bool value);

// ---------- SETUP ----------
void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(START_BUTTON_PIN, INPUT_PULLUP);

  randomSeed(analogRead(A0));
  disableAlliances();
}

// ---------- LOOP ----------
void loop() {
  if (digitalRead(START_BUTTON_PIN) == LOW) {
    unsigned long now = millis();
    if (now - lastButtonTime > DEBOUNCE_DELAY) {
      lastButtonTime = now;

      if (matchState == IDLE) startMatch();
      else resetMatch();
    }
  }

  if (matchState != IDLE) {
    updateMatch();
  }
}

// ---------- MATCH CONTROL ----------
void startMatch() {
  redFirst = random(0, 2);
  redTurn  = redFirst;
  teleopCycle = 0;

  matchState = AUTO;
  stageStartTime = millis();
}

void resetMatch() {
  matchState = IDLE;
  disableAlliances();
}

// ---------- MATCH SEQUENCER ----------
void updateMatch() {
  unsigned long now = millis();
  unsigned long elapsed = now - stageStartTime;

  switch (matchState) {

    case AUTO:
      enableAlliances();
      if (elapsed >= AUTO_TIME) {
        matchState = TRANSITION;
        stageStartTime = now;
      }
      break;

    case TRANSITION:
      // Blink the alliance that will be disabled first
      if (redFirst) {
        blinkBlue();
        digitalWriteInverted(RED_PIN, 1);  // keep red solid
      } else {
        blinkRed();
        digitalWriteInverted(BLUE_PIN, 1); // keep blue solid
      }

      if (elapsed >= TRANSITION_TIME) {
        matchState = TELEOP;
        stageStartTime = now;
      }
      break;

    case TELEOP: {
      bool warning = (teleopCycle < 3) &&
                     (elapsed >= (TELEOP_TIME - WARNING_TIME));

      if (redTurn) {
        if (warning) {
          blinkRed();
          digitalWriteInverted(BLUE_PIN, 0);
        } else {
          redAlliance();
        }
      } else {
        if (warning) {
          blinkBlue();
          digitalWriteInverted(RED_PIN, 0);
        } else {
          blueAlliance();
        }
      }

      if (elapsed >= TELEOP_TIME) {
        teleopCycle++;
        redTurn = !redTurn;
        stageStartTime = now;

        if (teleopCycle >= 4) {
          matchState = ENDGAME;
        }
      }
      break;
    }

    case ENDGAME:
      enableAlliances();
      if (elapsed >= ENDGAME_TIME) {
        resetMatch();
      }
      break;

    default:
      disableAlliances();
      break;
  }
}

// ---------- OUTPUT CONTROL ----------
void redAlliance() {
  digitalWriteInverted(RED_PIN, 1);
  digitalWriteInverted(BLUE_PIN, 0);
}

void blueAlliance() {
  digitalWriteInverted(RED_PIN, 0);
  digitalWriteInverted(BLUE_PIN, 1);
}

void enableAlliances() {
  digitalWriteInverted(RED_PIN, 1);
  digitalWriteInverted(BLUE_PIN, 1);
}

void disableAlliances() {
  digitalWriteInverted(RED_PIN, 0);
  digitalWriteInverted(BLUE_PIN, 0);
}

// ---------- BLINK HELPERS ----------
void blinkRed() {
  bool on = (millis() / BLINK_INTERVAL) % 2;
  digitalWriteInverted(RED_PIN, on);
}

void blinkBlue() {
  bool on = (millis() / BLINK_INTERVAL) % 2;
  digitalWriteInverted(BLUE_PIN, on);
}

void digitalWriteInverted(int pin, bool value) {
  digitalWrite(pin, !value);
}
