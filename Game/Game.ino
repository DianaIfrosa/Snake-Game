#include "Menu.h"

void setup() {
  Serial.begin(9600);

  loadSavedValues();
  matrixSetup();
  gameSetup();
  LCDSetup();
  joystickSetup();
}

void loop() {

  switch (gameState) {
    case GREETING:
      greeting();
      break;
    case MAIN_MENU:
      mainMenu();
      break;
    case MENU_OPTION:
      submenu();
      break;
    case SETTINGS:
      settingsMenu();
      break;
    case SETTINGS_OPTION:
      openSettingsOption();
      break;
    case GAME:
      game();
      break;
    case GAME_OVER:
      gameOver();
      break;
    case GAME_ACHIEVEMENTS:
      gameAchievements();
      break;
    case HIGHSCORE_ACHIEVED:
      highscoreAchieved();
      break;
    case ENTER_NAME:
      enterName();
      break;
    case PAUSE:
      pause();
      break;
  }
}

void loadSavedValues() {
  EEPROM.get(0, LCDBrightness);
  analogWrite(brightnessPin, LCDBrightValues[LCDBrightness]);
  
  EEPROM.get(1, matrixBrightness);
  lc.setIntensity(0, matrixBrightValues[matrixBrightness]);
  
  EEPROM.get(2, muted);
}
