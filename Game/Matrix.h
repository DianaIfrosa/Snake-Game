const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);  // DIN, CLK, LOAD, No. DRIVER

const byte matrixSize = 8;
byte matrix[matrixSize][matrixSize] = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 }
};  // snake will be marked with 2, obstacles with 1, food with 3 and blank spaces with 0

byte matrixBrightness = 0;
const byte matrixBrightValues[] = { 0, 5, 10, 15 };

const byte images[][8] = {
  { // play
    0b00000000,
    0b00000000,
    0b01111111,
    0b00111110,
    0b00011100,
    0b00001000,
    0b00000000,
    0b00000000 },
  { // highscore
    0b00000000,
    0b00111001,
    0b10101011,
    0b11111111,
    0b11111111,
    0b10101011,
    0b00111001,
    0b00000000 },
  { // settings
    0b00000000,
    0b00000000,
    0b11110100,
    0b10010010,
    0b10010010,
    0b01011110,
    0b00000000,
    0b00000000 },
  { // about me
    0b00000000,
    0b00000000,
    0b00100001,
    0b11010010,
    0b11111100,
    0b11010010,
    0b00100001,
    0b00000000 },
  { // how to play
    0b00000000,
    0b00000000,
    0b00100000,
    0b01000000,
    0b01001101,
    0b00110000,
    0b00000000,
    0b00000000 },
  { // greeting
    0b00111110,
    0b00001000,
    0b00111110,
    0b00000000,
    0b00101110,
    0b00000000,
    0b00111010,
    0b00000000 },
  { // game over
    0b00000000,
    0b10000010,
    0b01000100,
    0b00101000,
    0b00010000,
    0b00101000,
    0b01000100,
    0b10000010 },
  { // smiley face
    0b00000000,
    0b00001000,
    0b01100100,
    0b00000010,
    0b00000010,
    0b01100100,
    0b00001000,
    0b00000000 },
  { // difficulty
    0b00000000,
    0b00000010,
    0b00000000,
    0b00001110,
    0b00001110,
    0b00000000,
    0b00111110,
    0b00111110 },
  { // brightness
    0b10010010,
    0b01010100,
    0b00111000,
    0b11111111,
    0b00111000,
    0b01010100,
    0b10010010,
    0b00000000 },
  { // sound
    0b00001000,
    0b00011100,
    0b00111110,
    0b00000000,
    0b00101010,
    0b01001001,
    0b00001000,
    0b00000000 }
};

void matrixSetup() {
  lc.shutdown(0, false);  // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness);
  lc.clearDisplay(0);
}

void lightMatrix() {
  byte row;
  
  for (row = 0; row < matrixSize; row++) {
    lc.setRow(0, row, 0b11111111);
  }
}

void displayMatrixImage(byte imageNo) {
  byte row;
  
  for (row = 0; row < matrixSize; row++) {
    lc.setRow(0, row, images[imageNo][row]);
  }
}