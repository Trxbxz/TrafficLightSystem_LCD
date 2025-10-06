#include <LiquidCrystal.h>

// LCD pins: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

// Traffic LEDs
int red = 2;
int yellow = 3;
int green = 4;

// Pedestrian LEDs
int pedRed = 5;
int pedGreen = 6;

// Button
int button = 7;

// Timing (ms)
unsigned long redTime = 5000;
unsigned long greenTime = 5000;
unsigned long yellowTime = 2000;
unsigned long walkTime = 5000;

// State variables
enum TrafficState {RED, GREEN, YELLOW, PEDE_YELLOW, PEDE_WALK};
TrafficState state = RED;
unsigned long stateStart = 0;
bool pedWaiting = false;

void setup() {
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(pedRed, OUTPUT);
  pinMode(pedGreen, OUTPUT);
  pinMode(button, INPUT_PULLUP);

  digitalWrite(pedRed, HIGH);
  digitalWrite(pedGreen, LOW);

  lcd.begin(16, 2);
  lcd.print("Traffic System");

  stateStart = millis();
}

void loop() {
  unsigned long now = millis();

  // Check pedestrian button anytime during GREEN
  if (digitalRead(button) == LOW && state == GREEN) {
    pedWaiting = true;
    int remaining = (greenTime - (now - stateStart)) / 1000;
    if (remaining < 0) remaining = 0;
    lcd.setCursor(0, 1);
    lcd.print("Red in: ");
    lcd.print(remaining);
    lcd.print("s   ");
  }

  switch (state) {
    case RED:
      digitalWrite(red, HIGH);
      digitalWrite(yellow, LOW);
      digitalWrite(green, LOW);
      digitalWrite(pedRed, HIGH);
      digitalWrite(pedGreen, LOW);
      if (now - stateStart >= redTime) {
        state = GREEN;
        stateStart = now;
        pedWaiting = false;
        lcd.setCursor(0,1);
        lcd.print("                "); // clear line
      }
      break;

    case GREEN:
      digitalWrite(red, LOW);
      digitalWrite(yellow, LOW);
      digitalWrite(green, HIGH);
      if (now - stateStart >= greenTime) {
        if (pedWaiting) {
          state = PEDE_YELLOW; // add yellow before pedestrian walk
        } else {
          state = YELLOW;
        }
        stateStart = now;
      }
      break;

    case YELLOW:
      digitalWrite(green, LOW);
      digitalWrite(yellow, HIGH);
      if (now - stateStart >= yellowTime) {
        state = RED;
        stateStart = now;
      }
      break;

    case PEDE_YELLOW:
      // Yellow before pedestrian walk
      digitalWrite(green, LOW);
      digitalWrite(yellow, HIGH);
      digitalWrite(red, LOW);
      if (now - stateStart >= yellowTime) {
        state = PEDE_WALK;
        stateStart = now;
        digitalWrite(yellow, LOW);
        digitalWrite(red, HIGH); // red for traffic during pedestrian walk
      }
      break;

    case PEDE_WALK:
      digitalWrite(red, HIGH);
      digitalWrite(pedRed, LOW);
      digitalWrite(pedGreen, HIGH);
      if (now - stateStart >= walkTime) {
        digitalWrite(pedRed, HIGH);
        digitalWrite(pedGreen, LOW);
        state = GREEN;
        stateStart = now;
        pedWaiting = false;
      }
      break;
  }
}
