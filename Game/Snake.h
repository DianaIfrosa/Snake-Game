#include <LedControl.h>
#include "LCD.h"
#include "Joystick.h"
#include "Matrix.h"
#include "Buzzer.h"
#include <ArduinoQueue.h>

const byte yellowLedPin = A1;
const byte blueLedPin = 13;
const byte buttonPin = 8;

const byte initialLevel = 1;
const byte initialScore = 0;
const byte initialLives = 2;
const byte levelSpeed = 5;

const byte initialMoveInterval = 250;
const byte foodInterval = 150;
const byte joystickInterval = 50;

enum state { GREETING,
             MAIN_MENU,
             MENU_OPTION,
             SETTINGS,
             SETTINGS_OPTION,
             GAME,
             GAME_OVER,
             GAME_ACHIEVEMENTS,
             HIGHSCORE_ACHIEVED,
             ENTER_NAME,
             PAUSE };
state gameState = GREETING;

enum direction { HORIZONTAL_LEFT,
                 HORIZONTAL_RIGHT,
                 VERTICAL_UP,
                 VERTICAL_DOWN };
direction snakeDirection = HORIZONTAL_RIGHT;

struct point {
  byte x;
  byte y;
};

struct wall {
  point firstHalf;
  point secondHalf;
};

wall walls[] = { { { 0, 0 }, { 0, 0 } }, { { 0, 0 }, { 0, 0 } }, { { 0, 0 }, { 0, 0 } } };
ArduinoQueue<point> snakeBody(3 * matrixSize);

point snakeHead = { 0, 0 };
point snakeTail = { 0, 0 };
point food = { 0, 0 };
bool foodState = true;  // for blinking food

short speed = 250;
unsigned long previousMillisGame = 0;
unsigned long previousMillisJs = 0;
unsigned long previousMillisFood = 0;
unsigned long lastDebounceTimeButton = 0;
unsigned long gameStart = 0;

byte difficulty = 0;  // 0 - EASY, 1 - MEDIUM, 2 - HIGH
byte level = 1;
byte score = 0;
byte lives = 2;

const byte difficultyPoints[] = { 1, 2, 3 };  // points (per level) added to score for each level of difficulty
bool foodEaten = false;

byte buttonState = HIGH;
byte readingButton = HIGH;
byte lastReadingButton = HIGH;

bool canTeleportHorizontal = true;  // for teleporting through left and right sides of matrix
bool canTeleportVertical = false;   // for teleporting through up and down sides of matrix

// used to track snake behavior
bool teleportedHorizontal = false;
bool teleportedVertical = false;

void generateFood() {
  food.x = random(0, matrixSize);
  food.y = random(0, matrixSize);

  while (matrix[food.x][food.y] != 0) {
    food.x = random(0, matrixSize);
    food.y = random(0, matrixSize);
  }

  foodState = true;
  matrix[food.x][food.y] = 3;
  lc.setLed(0, food.x, food.y, true);
}

void generateWalls() {
  // first section: between rows 1, 2 and columns 2, 5, it will contain a horizontal wall made of 2 points
  walls[0].firstHalf.x = random(2, 5);
  walls[0].firstHalf.y = random(1, 3);

  walls[0].secondHalf.x = walls[0].firstHalf.x + 1;
  walls[0].secondHalf.y = walls[0].firstHalf.y;

  // second section: between rows 5, 7 and columns 5, 7, it will contain a vertical wall made of 2 points
  walls[1].firstHalf.x = random(5, 8);
  walls[1].firstHalf.y = random(5, 7);

  walls[1].secondHalf.x = walls[1].firstHalf.x;
  walls[1].secondHalf.y = walls[1].firstHalf.y + 1;

  // third section: between rows 5, 6 and columns 0, 3, it will contain a horizontal wall made of 2 points
  walls[2].firstHalf.x = random(0, 3);
  walls[2].firstHalf.y = random(5, 7);

  walls[2].secondHalf.x = walls[2].firstHalf.x + 1;
  walls[2].secondHalf.y = walls[2].firstHalf.y;
}

void moveSnakeHead() {
  if (snakeDirection == HORIZONTAL_LEFT || snakeDirection == HORIZONTAL_RIGHT) {
    if (joystickMovement == UP) {
      snakeDirection = VERTICAL_UP;
      if (snakeHead.y > 0) {
        snakeHead.y--;
      } else {
        snakeHead.y = matrixSize - 1;
        teleportedVertical = true;
      }
    } else if (joystickMovement == DOWN) {      
      snakeDirection = VERTICAL_DOWN;
      if (snakeHead.y < matrixSize - 1) {
        snakeHead.y++;
      } else {
        teleportedVertical = true;
        snakeHead.y = 0;
      }
    } else {  // moving straight
      if (snakeDirection == HORIZONTAL_LEFT) {
        if (snakeHead.x > 0) {
          snakeHead.x--;
        } else {
          snakeHead.x = matrixSize - 1;
          teleportedHorizontal = true;
        }
      } else {
        if (snakeHead.x < matrixSize - 1) {
          snakeHead.x++;
        } else {
          snakeHead.x = 0;
          teleportedHorizontal = true;
        }
      }
    }
  } else if (snakeDirection == VERTICAL_UP || snakeDirection == VERTICAL_DOWN) {
    if (joystickMovement == RIGHT) {
      snakeDirection = HORIZONTAL_RIGHT;
      if (snakeHead.x < matrixSize - 1) {
        snakeHead.x++;
      } else {
        snakeHead.x = 0;
        teleportedHorizontal = true;
      }
    } else if (joystickMovement == LEFT) {
      snakeDirection = HORIZONTAL_LEFT;
      if (snakeHead.x > 0) {
        snakeHead.x--;
      } else {
        snakeHead.x = matrixSize - 1;
        teleportedHorizontal = true;
      }
    } else {  // moving straight
      if (snakeDirection == VERTICAL_UP) {
        if (snakeHead.y > 0) {
          snakeHead.y--;
        } else {
          snakeHead.y = matrixSize - 1;
          teleportedVertical = true;
        }
      } else {
        if (snakeHead.y < matrixSize - 1) {
          snakeHead.y++;
        } else {
          snakeHead.y = 0;
          teleportedVertical = true;
        }
      }
    }
  }
}

void updateSnake() {
  matrix[snakeHead.x][snakeHead.y] = 2;
  snakeBody.enqueue({ snakeHead.x, snakeHead.y });
  lc.setLed(0, snakeHead.x, snakeHead.y, true);

  if (foodEaten == false) {
    snakeTail = snakeBody.dequeue();
    matrix[snakeTail.x][snakeTail.y] = 0;
    lc.setLed(0, snakeTail.x, snakeTail.y, false);
  }
}

byte getSnakeStatus() {
  // return 0 if nothing happened, 1 if food was eaten and 2 if snake lost a life

  if (difficulty == 2) {  // HIGH
    if (teleportedHorizontal && !canTeleportHorizontal) {
      return 2;
    } else if (teleportedVertical && !canTeleportVertical) {
      return 2;
    }
  }

  if (matrix[snakeHead.x][snakeHead.y] == 0) {
    return 0;
  } else if (matrix[snakeHead.x][snakeHead.y] == 3) {
    return 1;
  } else {  // snake crashed to an obstacle or to itself
    return 2;
  }
}

void markWalls() {
  byte i;
  
  for (i = 0; i < ARRAY_SIZE(walls); i++) {
    matrix[walls[i].firstHalf.x][walls[i].firstHalf.y] = 1;
    matrix[walls[i].secondHalf.x][walls[i].secondHalf.y] = 1;
  }
}

void displayWalls() {
  byte i;

  for (i = 0; i < ARRAY_SIZE(walls); i++) {
    lc.setLed(0, walls[i].firstHalf.x, walls[i].firstHalf.y, true);
    lc.setLed(0, walls[i].secondHalf.x, walls[i].secondHalf.y, true);
  }
}

void verifyButtonState() {
  digitalWrite(blueLedPin, canTeleportHorizontal);
  digitalWrite(yellowLedPin, canTeleportVertical);

  readingButton = digitalRead(buttonPin);

  if (readingButton != lastReadingButton) {
    lastDebounceTimeButton = millis();
  }

  currentMillis = millis();

  if (currentMillis - lastDebounceTimeButton > shortDebounceDelay) {
    if (readingButton != buttonState) {
      buttonState = readingButton;
      if (buttonState == HIGH) {
        canTeleportHorizontal = !canTeleportHorizontal;
        canTeleportVertical = !canTeleportVertical;
      }
    }
  }

  lastReadingButton = readingButton;
}

void restartGame(byte levelValue, byte scoreValue, byte livesValue) {
  byte row, col, letter;
  point snakePart;

  level = levelValue;
  score = scoreValue;
  lives = livesValue;

  while (!snakeBody.isEmpty()) {
    snakePart = snakeBody.dequeue();
  }

  lc.clearDisplay(0);
  for (row = 0; row < matrixSize; row++) {
    for (col = 0; col < matrixSize; col++) {
      matrix[row][col] = 0;
    }
  }

  if (lives == initialLives) {  // no lives were lost before
    for (letter = 0; letter < noLettersHighscore; letter++) {
      nameIntroduced[letter] = 'A';
    }

    if (difficulty) {  // difficulty is not EASY
      generateWalls();
    }
  }

  if (difficulty) {
    displayWalls();
    markWalls();
  }

  snakeHead.x = 1;
  snakeHead.y = 3;
  snakeTail.x = 0;
  snakeTail.y = 3;

  snakeBody.enqueue({ snakeTail.x, snakeTail.y });
  snakeBody.enqueue({ snakeHead.x, snakeHead.y });

  snakeDirection = HORIZONTAL_RIGHT;

  matrix[snakeHead.x][snakeHead.y] = 2;
  matrix[snakeTail.x][snakeTail.y] = 2;

  lc.setLed(0, snakeHead.x, snakeHead.y, true);
  lc.setLed(0, snakeTail.x, snakeTail.y, true);

  lcd.clear();
  displayGame(levelValue, scoreValue, livesValue);

  generateFood();
  foodEaten = false;
  speed = initialMoveInterval;

  previousMillisGame = 0;
  previousMillisJs = 0;
  previousMillisFood = 0;

  canTeleportHorizontal = true;
  canTeleportVertical = false;

  teleportedHorizontal = false;
  teleportedVertical = false;

  gameStart = millis();
}

void blinkFood() {
  currentMillis = millis();

  lc.setLed(0, food.x, food.y, foodState);
  if (previousMillisFood / foodInterval != (currentMillis - gameStart) / foodInterval) {
    previousMillisFood = currentMillis - gameStart;
    foodState = !foodState;
  }
}

void pause() {
  displayPauseMessage();
  
  checkJoystick();
  if (joystickMovement == SHORT_PRESS) { 
    gameState = GAME;
    lcd.clear();
    displayGame(level, score, lives);    
  }    
}

void game() {
  static byte status = 0;
  oldJoystickMovement = joystickMovement;

  currentMillis = millis();

  if (previousMillisJs / joystickInterval != (currentMillis - gameStart) / joystickInterval) {
    previousMillisJs = currentMillis - gameStart;
    checkJoystick();

    if (!(joystickMovement != oldJoystickMovement && joystickMovement != NONE))  // get first movement different than NONE for the joystick
      joystickMovement = oldJoystickMovement;
  }

  if (difficulty == 2) {
    verifyButtonState();
  }

  if (joystickMovement == LONG_PRESS) {  // pausing the game
    gameState = PAUSE;
    lcd.clear();
    
  } else {
    blinkFood();

    if (previousMillisGame / speed != (currentMillis - gameStart) / speed) {
      previousMillisGame = currentMillis - gameStart;

      moveSnakeHead();
      joystickMovement = NONE;
      status = getSnakeStatus();
      if (status == 0) {  // nothing happened
        updateSnake();
        teleportedHorizontal = false;
        teleportedVertical = false;

      } else if (status == 1) {  // ate food
        foodEaten = true;
        updateSnake();
        eatSound();
        foodEaten = false;
        generateFood();
        score += difficultyPoints[difficulty];
        level++;
        speed -= levelSpeed;
        teleportedHorizontal = false;
        teleportedVertical = false;
        updateGameDisplay(level, score, lives, initialLevel, initialScore, initialLives);

      } else {  // lost a life
        lives--;
        updateGameDisplay(level, score, lives, initialLevel, initialScore, initialLives);
        lifeLostSound();
        if (lives == 0) {
          gameState = GAME_OVER;
          lcd.clear();
        } else {
          restartGame(level, score, lives);
        }
      }
    }
  }
}

void gameOver() {
  static byte message = ARRAY_SIZE(gameOverMessages);

  lc.clearDisplay(0);
  displayMatrixImage(6);

  if (message == ARRAY_SIZE(gameOverMessages)) {  // choose only one message per game ended
    message = random(0, ARRAY_SIZE(gameOverMessages));
  }

  checkJoystick();
  displayGameOver(message);

  if (joystickMovement == SHORT_PRESS) {
    message = ARRAY_SIZE(gameOverMessages);
    gameState = GAME_ACHIEVEMENTS;
    digitalWrite(blueLedPin, false);
    digitalWrite(yellowLedPin, false);
    lcd.clear();
  }
}

void highscoreAchieved() {
  checkJoystick();
  if (joystickMovement == SHORT_PRESS) {
    gameState = ENTER_NAME;
    lcd.clear();
    displayEnterName(0);
  }
}

void gameAchievements() {
  displayGameAchievements(score, level);

  checkJoystick();
  if (joystickMovement == SHORT_PRESS) {
    if (score > getHighscoreValue(noHighscores)) {  // compare with lowest highscore
      gameState = HIGHSCORE_ACHIEVED;
      lc.clearDisplay(0);
      lcd.clear();
      displayMatrixImage(7);
      displayHighscoreCongrats();
    } else {
      gameState = MAIN_MENU;
      lcd.clear();
      lc.clearDisplay(0);
    }
  }
}

void updateHighscoreList() {
  byte highscore, pos, i, oldScore;
  char* oldName;

  for (pos = 5; pos >= 1; pos--) {
    highscore = getHighscoreValue(pos);
    if (score <= highscore) {
      break;
    }
  }
  pos++;

  // shift the other highscores
  for (i = noHighscores - 1; i >= pos; i--) {
    oldScore = getHighscoreValue(i);
    oldName = getHighscoreName(i);
    writeHighscore(i + 1, oldScore, oldName);
    free(oldName);
  }

  writeHighscore(pos, score, nameIntroduced);
}

void enterName() {
  static byte letter = 0;
  checkJoystick();

  if (joystickMovement == LEFT && letter > 0) {
    letter--;
    menuSound();
    displayEnterName(letter);

  } else if (joystickMovement == RIGHT && letter < noLettersHighscore - 1) {
    letter++;
    menuSound();
    displayEnterName(letter);

  } else if (joystickMovement == UP) {
    menuSound();
    if (nameIntroduced[letter] == 'Z') {
      nameIntroduced[letter] = 'A';
    } else {
      nameIntroduced[letter]++;
    }
    updateEnterName();

  } else if (joystickMovement == DOWN) {
    menuSound();
    if (nameIntroduced[letter] == 'A') {
      nameIntroduced[letter] = 'Z';
    } else {
      nameIntroduced[letter]--;
    }
    updateEnterName();

  } else if (joystickMovement == SHORT_PRESS) {
    updateHighscoreList();

    menuSound();
    letter = 0;
    gameState = MAIN_MENU;

    lcd.clear();
    lc.clearDisplay(0);
  }
}

void resetHighscore() {
  for (byte i = 1; i <= noHighscores; i++) {
    writeHighscore(i, 0, "NONE ");
  }
}

void gameSetup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(yellowLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);

  gameState = GREETING;
  randomSeed(analogRead(3));
}