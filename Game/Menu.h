#include "Snake.h"

const unsigned short scrollingTextInterval = 350;
const unsigned short greetingTime = 2000;

unsigned long previousMillis = 0;
unsigned long submenuStartMillis = 0;

byte menuOption = 0;
byte settingsOption = 0;

void play() {
  gameState = GAME;
  restartGame(initialLevel, initialScore, initialLives);
}

void highscore() {
  static byte position = 0;

  displayHighscoreList(position + 1);
  checkJoystick();

  if (joystickMovement == LEFT) {
    gameState = MAIN_MENU;
    position = 0;
    menuSound();
    lcd.clear();
    
  } else if (joystickMovement == UP && position > 0) {
    position--;
    menuSound();
    lcd.clear();

  } else if (joystickMovement == DOWN && position < noHighscores) {
    position++;
    menuSound();
    lcd.clear();

  } else if (joystickMovement == SHORT_PRESS && position == noHighscores) {
    gameState = MAIN_MENU;
    position = 0;
    menuSound();
    lcd.clear();
    resetHighscore();
  }
}

void about() {
  static byte infoPosition = 0;  // used for scrolling text
  checkJoystick();

  if (joystickMovement == LEFT) {
    gameState = MAIN_MENU;
    menuSound();
    lcd.clear();

  } else {
    displayStaticText("Snake game", false);
    displayLetters(0, infoPosition, scrollingInfo);

    currentMillis = millis();
    if (previousMillis / scrollingTextInterval != (currentMillis - submenuStartMillis) / scrollingTextInterval) {
      previousMillis = currentMillis - submenuStartMillis;
      infoPosition++;
      if (infoPosition > strlen(scrollingInfo) - displayLength) {
        infoPosition = 0;
      }
      lcd.clear();
    }
  }
}

void howToPlay() {
  static byte textPosition = 0;  // used for scrolling text
  static byte currentDifficulty = 0;
  char* text;
  checkJoystick();

  if (joystickMovement == LEFT && currentDifficulty == 0) {
    gameState = MAIN_MENU;
    menuSound();
    lcd.clear();

  } else if (joystickMovement == LEFT && currentDifficulty > 0) {
    textPosition = 0;
    currentDifficulty--;
    menuSound();
    lcd.clear();

  } else if (joystickMovement == RIGHT) {
    textPosition = 0;
    currentDifficulty++;
    if (currentDifficulty > 2) {
      currentDifficulty = 0;
    }
    menuSound();
    lcd.clear();
    
  } else {
    displayStaticText(difficultyOptions[currentDifficulty], true);
    switch (currentDifficulty) {
      case 0:
        text = scrollingInstructionsEasy;
        break;
      case 1:
        text = scrollingInstructionsMedium;
        break;
      case 2:
        text = scrollingInstructionsHard;
        break;
    }

    displayLetters(0, textPosition, text);

    currentMillis = millis();
    if (previousMillis / scrollingTextInterval != (currentMillis - submenuStartMillis) / scrollingTextInterval) {
      previousMillis = currentMillis - submenuStartMillis;
      textPosition++;
      if (textPosition > strlen(text) - displayLength) {
        textPosition = 0;
        currentDifficulty++;
        if (currentDifficulty > 2) {
          currentDifficulty = 0;
        }
      }
      lcd.clear();
    }
  }
}

void setDifficulty() {
  checkJoystick();
  displayDifficulty(difficulty);

  if (joystickMovement == UP && difficulty > 0) {
    difficulty -= 1;
    menuSound();
    lcd.clear();

  } else if (joystickMovement == DOWN && difficulty < ARRAY_SIZE(difficultyOptions) - 1) {
    difficulty += 1;
    menuSound();
    lcd.clear();

  } else if (joystickMovement == LEFT) {  // exit the menu option & save value to EEPROM
    gameState = SETTINGS;
    menuSound();
    lcd.clear();
  }
}

void setLCDBrightness() {
  displayBrightness(LCDBrightness, ARRAY_SIZE(LCDBrightValues));

  checkJoystick();
  if (joystickMovement == UP) {
    if (LCDBrightness < ARRAY_SIZE(LCDBrightValues) - 1) {
      LCDBrightness += 1;
      menuSound();
      analogWrite(brightnessPin, LCDBrightValues[LCDBrightness]);
      updateBrightnessDisplay(LCDBrightness);
    }
  } else if (joystickMovement == DOWN) {
    if (LCDBrightness > 0) {
      LCDBrightness -= 1;
      menuSound();
      analogWrite(brightnessPin, LCDBrightValues[LCDBrightness]);
      updateBrightnessDisplay(LCDBrightness);
    }
  } else if (joystickMovement == LEFT) {  // exit the menu option & save value to EEPROM
    EEPROM.put(0, LCDBrightness);
    gameState = SETTINGS;
    menuSound();
    lcd.clear();
  }
}

void setMatrixBrightness() {
  static bool matrixOn = false;

  if (!matrixOn) {
    matrixOn = true;
    lightMatrix();
  }

  displayBrightness(matrixBrightness, ARRAY_SIZE(matrixBrightValues));
  checkJoystick();
  
  if (joystickMovement == UP) {
    if (matrixBrightness < ARRAY_SIZE(matrixBrightValues) - 1) {
      matrixBrightness += 1;
      menuSound();
      lc.setIntensity(0, matrixBrightValues[matrixBrightness]);
      updateBrightnessDisplay(matrixBrightness);
    }
  } else if (joystickMovement == DOWN) {
    if (matrixBrightness > 0) {
      matrixBrightness -= 1;
      menuSound();
      lc.setIntensity(0, matrixBrightValues[matrixBrightness]);
      updateBrightnessDisplay(matrixBrightness);
    }
  } else if (joystickMovement == LEFT) {  // exit the menu option & save value to EEPROM
    EEPROM.put(1, matrixBrightness);
    gameState = SETTINGS;
    matrixOn = false;
    menuSound();
    lcd.clear();
    lc.clearDisplay(0);
    displayMatrixImage(9);
  }
}

void setSound() {
  checkJoystick();

  if (joystickMovement == UP || joystickMovement == DOWN) {
    muted = !muted;
    updateSoundDisplay(muted);
    
  } else if (joystickMovement == LEFT) {  // exit the menu option & save value to EEPROM
    EEPROM.put(2, muted);
    gameState = SETTINGS;
    menuSound();
    lcd.clear();
  }
}

void openSettingsOption() {
  switch (settingsOption) {
    case 0:
      setDifficulty();
      break;
    case 1:
      setLCDBrightness();
      break;
    case 2:
      setMatrixBrightness();
      break;
    case 3:
      displaySound(muted);
      setSound();
      break;
  }
}

void settingsMenu() {
  displaySettingsMenu(settingsOption);

  if (settingsOption == 0) {
    displayMatrixImage(8);
  } else if (settingsOption == 1 || settingsOption == 2) {
    displayMatrixImage(9);
  } else {
    displayMatrixImage(10);
  }

  checkJoystick();
  if (joystickMovement == RIGHT) {
    gameState = SETTINGS_OPTION;
    menuSound();
    lcd.clear();

  } else {
    if (joystickMovement == UP && settingsOption > 0) {
      settingsOption--;
      menuSound();
      lcd.clear();

    } else if (joystickMovement == DOWN && settingsOption < ARRAY_SIZE(settingsOptions) - 1) {
      settingsOption++;
      menuSound();
      lcd.clear();
      
    } else if (joystickMovement == LEFT) {
      gameState = MAIN_MENU;
      menuSound();
      lcd.clear();
    }
  }
}

void submenu() {
  switch (menuOption) {
    case 0:
      play();
      break;
    case 1:
      highscore();
      break;
    case 2:
      settingsMenu();
      break;
    case 3:
      about();
      break;
    case 4:
      howToPlay();
      break;
  }
}

void mainMenu() {
  displayMenu(menuOption);
  displayMatrixImage(menuOption);

  checkJoystick();
  if (joystickMovement == RIGHT) {
    gameState = MENU_OPTION;
    previousMillis = 0;
    submenuStartMillis = millis();
    menuSound();
    lcd.clear();

  } else {
    if (joystickMovement == UP) {
      if (menuOption > 0) {
        menuOption--;
        menuSound();
        lcd.clear();
      }
    } else if (joystickMovement == DOWN) {
      if (menuOption < ARRAY_SIZE(options) - 1) {
        menuOption++;
        menuSound();
        lcd.clear();
      }
    }
  }
}

void greeting() {
  currentMillis = millis();

  if (currentMillis < greetingTime) {
    lcd.setCursor(5, 0);
    lcd.print("Hello!");
    displayMatrixImage(5);    
  } else {
    gameState = MAIN_MENU;
    lcd.clear();
  }
}