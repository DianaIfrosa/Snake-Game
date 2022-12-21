#define NOTE_A4  440
#define NOTE_G1  49
#define NOTE_GS2 104
#define NOTE_C1  33
#define NOTE_B7  3951

const byte buzzerPin = A2;

const byte menuSoundDuration = 100;
const byte eatSoundDuration = 100;
const byte lifeLostSoundDuration = 200;

bool muted = false;

void menuSound() {
  if (!muted) {
    tone(buzzerPin, NOTE_A4, menuSoundDuration);
  }
}

void eatSound() {
   if (!muted) {
    tone(buzzerPin, NOTE_G1, eatSoundDuration);
    tone(buzzerPin, NOTE_GS2, eatSoundDuration);
  }  
}

void lifeLostSound() {
    if (!muted) {
    tone(buzzerPin, NOTE_B7, lifeLostSoundDuration);
    tone(buzzerPin, NOTE_C1, lifeLostSoundDuration);
  }   
}