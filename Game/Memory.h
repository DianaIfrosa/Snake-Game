#include <EEPROM.h>

const byte highscoreStartPosition = 3;
const byte noLettersHighscore = 5;
const byte lengthHighscorePair = 7;
const byte noHighscores = 5;

char* readStringFromMemory(byte addrOffset) {
  byte readStrLen = EEPROM.read(addrOffset);
  byte i;
  char character;
  char* text = (char*) malloc(readStrLen + 1);
  
  for (i = 0; i < readStrLen; i++) {
    text[i] = char(EEPROM.read(addrOffset + 1 + i));
  }

  text[readStrLen] = '\0';
  return text;
}

void writeStringToMemory(byte addrOffset, char* text) {
  byte i;

  EEPROM.put(addrOffset, noLettersHighscore);
  for (i = 0; i < noLettersHighscore; i++) {
    EEPROM.put(addrOffset + 1 + i, text[i]);
  }
} 

byte getHighscoreValue(byte no) {
  byte position = highscoreStartPosition + (noHighscores - no) * lengthHighscorePair + noLettersHighscore + 1;
  
  return EEPROM.read(position);
}

char* getHighscoreName(byte no) {
  byte position = highscoreStartPosition + (noHighscores - no) * lengthHighscorePair;
  char* name = readStringFromMemory(position);
  
  return name;
}

void writeHighscore(byte no, byte score, char* name) {
  byte position = highscoreStartPosition + (noHighscores - no) * lengthHighscorePair;
  
  writeStringToMemory(position, name);
  position += noLettersHighscore + 1;
  EEPROM.update(position, score);
}