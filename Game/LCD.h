#include <LiquidCrystal.h>
#include "Memory.h"

#define ARRAY_SIZE(x) sizeof(x) / sizeof(x[0])

const byte RS = 6;
const byte enable = 2;
const byte d4 = 7;
const byte d5 = A0;
const byte d6 = 5;
const byte d7 = 4;
const byte brightnessPin = 9;

LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);

const byte displayLength = 16;
const byte LCDBrightValues[] = { 10, 90, 170, 255 };
byte LCDBrightness = 0;

const char options[][12] = { "Start game", "Highscore", "Settings", "About", "How to play" };

char scrollingInstructionsEasy[] = "                move using joystick, eat food to increase score, you can teleport through walls               ";
char scrollingInstructionsMedium[] =  "                EASY rules + avoid the fixed walls              ";
char scrollingInstructionsHard[] = "                MEDIUM rules + can teleport through walls only if the corresponding LEDs are ON (blue - left & right, yellow - up & down). Press the button to switch the LEDs               ";

const char scrollingInfo[] = "                by Ifrosa Diana-Maria, github.com/DianaIfrosa               ";
const char settingsOptions[][15] = { "Difficulty", "LCD brightness", "MTR brightness", "Sound" };
const char gameOverMessages[][17] = { "Nice game!", "Don't give up!", "Keep it up!", "You got this!", "Great work!" };
const char difficultyOptions[][7] = { "EASY", "MEDIUM", "HARD" };
char nameIntroduced[] = "AAAAA";

byte rightArrow[] = {
  B00000,
  B00100,
  B00010,
  B11111,
  B00010,
  B00100,
  B00000,
  B00000
};

byte leftArrow[] = {
  B00000,
  B00100,
  B01000,
  B11111,
  B01000,
  B00100,
  B00000,
  B00000
};

byte downArrow[] = {
  B00000,
  B00000,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000,
  B00000
};

byte upArrow[] = {
  B00000,
  B00000,
  B00100,
  B01110,
  B11111,
  B00000,
  B00000,
  B00000
};

byte heart[] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000
};

byte upDownArrow[] = {
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b00100,
  0b10101,
  0b01110,
  0b00100
};

byte bar[] = {
  0b11111,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

void createLCDSymbols() {
  lcd.createChar(0, rightArrow);
  lcd.createChar(1, leftArrow);
  lcd.createChar(2, downArrow);
  lcd.createChar(3, upArrow);
  lcd.createChar(4, heart);
  lcd.createChar(5, upDownArrow);
  lcd.createChar(6, bar);
}

void LCDSetup() {
  lcd.begin(16, 2); 
  createLCDSymbols();
}

void displayMenu(byte currentOption) {
  lcd.setCursor(0, 0);
  lcd.write(byte(0));  // right arrow

  lcd.setCursor(1, 0);
  lcd.print(options[currentOption]);

  if (currentOption + 1 < ARRAY_SIZE(options)) {
    lcd.setCursor(1, 1);
    lcd.print(options[currentOption + 1]);

    lcd.setCursor(15, 1);
    lcd.write(byte(2));  // down arrow
  }

  if (currentOption > 0) {
    lcd.setCursor(15, 0);
    lcd.write(byte(3));  // up arrow
  }
}

void displayHighscoreList(byte position) {
  // display a top 5 players with name and score, and the last option should be to reset the highscore (press)
  byte currentScore;
  char* currentName;

  lcd.setCursor(0, 0);
  lcd.write(byte(0));  // right arrow

  // display arrows
  if (position == 1) {
    lcd.setCursor(15, 0);
    lcd.write(byte(2));  // down arrow
  }
  
  if (position > 1) {
    lcd.setCursor(15, 0);
    lcd.write(byte(3));  // up arrow

    if (position <= noHighscores) {
      lcd.setCursor(15, 1);
      lcd.write(byte(2));  // down arrow
    }
  }

  if (position <= noHighscores) {
    // display the current highscore on first line of LCD
    lcd.setCursor(1, 0);
    lcd.print(position);
    lcd.print(". ");
    currentName = getHighscoreName(position);
    lcd.print(currentName);
    free(currentName);
    lcd.print(" ");
    currentScore = getHighscoreValue(position);
    lcd.print(currentScore);
  }

  if (position <= noHighscores - 1) {
    // display the following highscore on second line of LCD
    lcd.setCursor(1, 1);
    lcd.print(position + 1);
    lcd.print(". ");
    currentName = getHighscoreName(position + 1);
    lcd.print(currentName);
    free(currentName);
    lcd.print(" ");
    currentScore = getHighscoreValue(position + 1);
    lcd.print(currentScore);

  } else if (position == noHighscores) {
    lcd.setCursor(1, 1);
    lcd.print("Reset list");
    
  } else if (position == noHighscores + 1) {
    lcd.setCursor(1, 0);
    lcd.print("Reset list");
    lcd.setCursor(1, 1);
    lcd.print("(push button)");
  }
}

void displaySettingsMenu(byte currentOption) {
  lcd.setCursor(0, 0);
  lcd.write(byte(0));  // right arrow

  lcd.setCursor(1, 0);
  lcd.print(settingsOptions[currentOption]);

  if (currentOption + 1 < ARRAY_SIZE(settingsOptions)) {
    lcd.setCursor(1, 1);
    lcd.print(settingsOptions[currentOption + 1]);

    lcd.setCursor(15, 1);
    lcd.write(byte(2));  // down arrow
  }

  if (currentOption > 0) {
    lcd.setCursor(15, 0);
    lcd.write(byte(3));  // up arrow
  }

  if (currentOption == ARRAY_SIZE(settingsOptions) - 1) {
    lcd.setCursor(0, 1);
    lcd.print("Back:scroll left");
  }
}

void displaySound(bool muted) {
  lcd.setCursor(0, 0);
  lcd.write(byte(1));  // left arrow
  lcd.setCursor(3, 0);
  lcd.print("Sound: ");
  lcd.setCursor(10, 0);

  if (muted) {
    lcd.print("OFF");
  } else {
    lcd.print("ON");
  }
  
  lcd.setCursor(14, 0);
  lcd.write(byte(5));  // up-down arrow
}

void updateSoundDisplay(bool muted) {
  lcd.setCursor(10, 0);
  
  if (muted) {
    lcd.print("OFF");
  } else {
    lcd.print("ON ");
  }
}

void displayBrightness(byte currentValue, byte maxValue) {
  // this function is used for LCD and Matrix brightness display

  lcd.setCursor(0, 0);
  lcd.write(byte(1));  // left arrow

  // display "currentValue/maxValue", where currentValue is [1,4]
  lcd.setCursor(2, 0);
  lcd.print("Brightness:");
  lcd.setCursor(7, 1);
  lcd.print(currentValue + 1);
  lcd.print("/");
  lcd.print(maxValue);

  lcd.setCursor(11, 1);
  lcd.write(byte(5));  // up-down arrow
}

void displayDifficulty(byte option) {
  lcd.setCursor(0, 0);
  lcd.write(byte(0));  // right arrow

  lcd.setCursor(1, 0);
  lcd.print(difficultyOptions[option]);

  if (option + 1 < ARRAY_SIZE(difficultyOptions)) {
    lcd.setCursor(1, 1);
    lcd.print(difficultyOptions[option + 1]);

    lcd.setCursor(15, 1);
    lcd.write(byte(2));  // down arrow
  }

  if (option > 0) {
    lcd.setCursor(15, 0);
    lcd.write(byte(3));  // up arrow
  }
}

void updateBrightnessDisplay(byte brightness) {
  lcd.setCursor(7, 1);
  lcd.print(brightness + 1);
}

void displayLetters(byte startPrint, byte startLetter, char scrollingText[]) {
  lcd.setCursor(startPrint, 1);

  // print only 16 chars in second line starting with 'startLetter'
  for (byte letter = startLetter; letter < startLetter + displayLength; letter++) {
    lcd.print(scrollingText[letter]);
  }
}

void displayStaticText(char text[], bool canSkip) {
  lcd.setCursor(0, 0);
  lcd.write(byte(1));  // left arrow

  lcd.setCursor(2, 0);
  lcd.print(text);

  // added boolean parameter to use the function for both 'About' and 'How to play' sections
  // in the 'How to play' section, the player can navigate between 3 levels of difficulty
  
  if (canSkip) {
    lcd.setCursor(15, 0);
    lcd.write(byte(0)); // right arrow
  }
}

void displayGame(byte level, byte score, byte lives) {
  lcd.setCursor(4, 0);
  lcd.print("Score:");
  lcd.print(score);

  lcd.setCursor(1, 1);
  lcd.print("Level:");
  lcd.print(level);

  lcd.setCursor(10, 1);
  lcd.write(byte(4));  // heart
  lcd.print(":");
  lcd.print(lives);
}

void updateGameDisplay(byte level, byte score, byte lives, byte initialLevel, byte initialScore, byte initialLives) {
  if (level != initialLevel) {
    lcd.setCursor(7, 1);
    lcd.print(level);
  }

  if (score != initialScore) {
    lcd.setCursor(10, 0);
    lcd.print(score);
  }

  if (lives != initialLives) {
    lcd.setCursor(12, 1);
    lcd.print(lives);
  }
}

void displayGameOver(byte messageNo) {
  lcd.setCursor(1, 0);
  lcd.print(gameOverMessages[messageNo]);

  lcd.setCursor(1, 1);
  lcd.print("Press for more");
}

void displayGameAchievements(byte score, byte level) {
  lcd.setCursor(3, 0);
  lcd.print("Level: ");
  lcd.print(level);

  lcd.setCursor(3, 1);
  lcd.print("Score: ");
  lcd.print(score);
}

void displayHighscoreCongrats() {
  lcd.setCursor(1, 0);
  lcd.print("Congrats! You");

  lcd.setCursor(1, 1);
  lcd.print("are in top 5!");
}

void displayEnterName(byte positionBar) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Name: ");
  lcd.print(nameIntroduced);
 
  lcd.setCursor(15, 0);
  lcd.write(byte(5));  // up-down arrow

  // mark the current letter selected
  lcd.setCursor(6 + positionBar, 1);
  lcd.write(byte(6));  // bar
}

void updateEnterName() {
  lcd.setCursor(6, 0);
  lcd.print(nameIntroduced);
}

void displayPauseMessage() {
  lcd.setCursor(2, 0);
  lcd.print("Game paused");

  lcd.setCursor(1, 1);
  lcd.print("Press to play");
}