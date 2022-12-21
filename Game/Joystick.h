const byte pinSW = A5;
const byte pinX = A3;
const byte pinY = A4;

byte swState = HIGH;
byte readingJoystick = HIGH;
byte lastReadingJoystick = HIGH;

bool joyMoved = false;
short  jsMinThreshold = 400;
short jsMaxThreshold = 600;

short xValue = 500;
short yValue = 500;

unsigned long currentMillis = 0;
unsigned long lastDebounceTimeJs = 0;
const byte shortDebounceDelay = 30;
const unsigned short longDebounceDelay = 1500;

enum jsMovement { NONE,
                  UP,
                  RIGHT,
                  DOWN,
                  LEFT,
                  SHORT_PRESS,
                  LONG_PRESS };

jsMovement joystickMovement = NONE;
jsMovement oldJoystickMovement = NONE;

short shortPress = -1;  // -1 = N/A, 0 = didn't occur, 1 = did occur
short longPress = -1;   // -1 = N/A, 0 = didn't occur, 1 = did occur

void checkJoystick() {
  // return value -> 0 (no movement), 1 (up), 2 (right), 3 (down), 4 (left), 5 (short pressed), 6 (long pressed)
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  joystickMovement = NONE;

  if (xValue >= jsMinThreshold && xValue <= jsMaxThreshold && yValue >= jsMinThreshold && yValue <= jsMaxThreshold) {
    joyMoved = false;
  }

  if (joyMoved == false && yValue < jsMinThreshold) {
    joyMoved = true;
    joystickMovement = UP;
  }

  if (joyMoved == false && xValue > jsMaxThreshold) {
    joyMoved = true;
    joystickMovement = RIGHT;
  }

  if (joyMoved == false && yValue > jsMaxThreshold) {
    joyMoved = true;
    joystickMovement = DOWN;
  }

  if (joyMoved == false && xValue < jsMinThreshold) {
    joyMoved = true;
    joystickMovement = LEFT;
  }

  readingJoystick = digitalRead(pinSW);

  if (readingJoystick != lastReadingJoystick) {
    lastDebounceTimeJs = millis();
  }

  currentMillis = millis();

  if (currentMillis - lastDebounceTimeJs > shortDebounceDelay) {  // check for short press
    if (readingJoystick != swState) {
      swState = readingJoystick;
      if (swState == LOW) {
        shortPress = 1;
      }
    }
  }

  currentMillis = millis();
  if (currentMillis - lastDebounceTimeJs > longDebounceDelay) {  // check for long press
    if (readingJoystick == swState && shortPress == 1) {
      if (swState == LOW) {
        longPress = 1;
      } else
        longPress = 0;
    }
  }

  if (shortPress == 1 && longPress == 0) {
    shortPress = -1;
    longPress = -1;
    joystickMovement = SHORT_PRESS;
  }

  if (shortPress == 1 && longPress == 1) {
    shortPress = -1;
    longPress = -1;
    joystickMovement = LONG_PRESS;
  }

  lastReadingJoystick = readingJoystick;
}

void joystickSetup() {
  pinMode(pinSW, INPUT_PULLUP); 
}