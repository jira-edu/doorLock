#include <Arduino.h>

int row[4] = {6, 7, 8, 9};
int col[3] = {A1, A2, A3};
char key;
int beep = 13;
int lock = 12;
String validPass = "77";
String bufPass;
bool f_lock;

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

uint8_t dot[8]  = {0x00,0x00,0x0E,0x1F,0x1F,0x0E,0x00,0x00};
LiquidCrystal_I2C lcd(0x27,16,2);

#include <note.h>


char keyScan(void);
void displayKeyPress(String Pass);
void displayPass(void);
void displayUnlock(void);
void displayDeny(void);
void displayIdle(void);
void displayLock(void);
void playWinSound(void);
void playFailSound(void);

void setup() {
  pinMode(beep, OUTPUT);
  pinMode(lock, OUTPUT);  
  digitalWrite(beep, LOW);
  digitalWrite(lock, HIGH);
  f_lock = true;

  Serial.begin(9600);

  for (int i=0; i<4; i++) {
    pinMode(row[i], OUTPUT);
    digitalWrite(row[i], HIGH);
  }
  for (int i=0; i<3; i++) {
    pinMode(col[i], INPUT_PULLUP);
  }

  lcd.init();
  lcd.backlight();
  lcd.createChar(0, dot);
  displayIdle();
}

void loop() {
  key = keyScan();
  delay(100);
  if (key != '-') {
    if (key == '#' && f_lock == true) {
      if (bufPass == validPass) {
        Serial.println("\nPassed");
        f_lock = false;
        digitalWrite(lock, LOW);
        // tone(beep, 1000);
        // delay(500);
        // noTone(beep);
        displayUnlock();
        playWinSound();
      } else {
        displayDeny();
        playFailSound();
        delay(5000);
        displayIdle();
        Serial.println("\nAccess Denied");
      }
      bufPass = "";
    } else if (key == '*') {
      bufPass = "";
      if (f_lock == false) {  
        digitalWrite(lock, HIGH);
        f_lock = true;
        displayLock();
        Serial.println("\nLocked");
        delay(2000);
        displayIdle();
      } else if (f_lock == true) {
        displayKeyPress("");
      }
      
    } else if (f_lock == true){
      bufPass += key;
      Serial.print("*");
      displayKeyPress(bufPass);
    }
    
  }
}

char keyScan() {
  int controlScan[4][4] = {{0,1,1,1},{1,0,1,1},{1,1,0,1},{1,1,1,0}};
  char key[4][3] = {{'1','2','3'},{'4','5','6'},{'7','8','9'},{'*','0','#'}};
  for (int i=0; i<4; i++) {
    for (int j=0; j<4; j++) {
      digitalWrite(row[j], controlScan[i][j]);
    }
    for (int k=0; k<3; k++) {
      if (digitalRead(col[k]) == LOW) {
        while (digitalRead(col[k]) == LOW) {
          tone(beep, 1000);
          delay(50); 
        }
        noTone(beep);
        return key[i][k];
      }
    }
  }
  return '-';
}

void displayKeyPress(String Pass) {
  int size = Pass.length();
  int beginPosition = 8 - (size / 2); 
  lcd.setCursor(0, 1);
  lcd.print("                ");
  if (size > 0) {
    lcd.setCursor(beginPosition, 1);
    for (int i=0; i<(size-1); i++) {
      lcd.printByte(0);
    }
    delay(150);
    lcd.printByte(0);
  }
}

void displayUnlock() {
  lcd.clear();
  lcd.home();
  lcd.print(" Access Allowed ");
}

void displayDeny() {
  lcd.clear();
  lcd.home();
  lcd.print(" Access Denied ");
}

void displayIdle() {
  lcd.clear();
  lcd.home();
  lcd.print(" Enter Password ");
}

void displayLock() {
  lcd.clear();
  lcd.home();
  lcd.print("     locked     ");
}

void playWinSound() {
  tone(beep, NOTE_E7, 200);
  delay(250);
  tone(beep, NOTE_G7, 200);
  delay(250);
  tone(beep, NOTE_E7, 200);
  delay(250);
  tone(beep, NOTE_C7, 200);
  delay(250);
  tone(beep, NOTE_D7, 200);
  delay(250);
  tone(beep, NOTE_G6, 400);
  delay(400);
  noTone(beep); // Stop the buzzer
}

void playFailSound() {
  tone(beep, NOTE_E5, 200);
  delay(250);
  tone(beep, NOTE_E4, 200);
  delay(250);
  tone(beep, NOTE_A4, 200);
  delay(250);
  noTone(beep); // Stop the buzzer
}