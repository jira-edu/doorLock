#include <Arduino.h>

int row[4] = {6, 7, 8, 9};
int col[3] = {A1, A2, A3};
char key;
int beep = 13;
int lock = 12;
String validPass = "456789";
String bufPass;

// put function declarations here:
char keyScan(void);

void setup() {
  Serial.begin(9600);

  for (int i=0; i<4; i++) {
    pinMode(row[i], OUTPUT);
    digitalWrite(row[i], HIGH);
  }
  for (int i=0; i<3; i++) {
    pinMode(col[i], INPUT_PULLUP);
  }

  pinMode(beep, OUTPUT);
  pinMode(lock, OUTPUT);  
  digitalWrite(lock, HIGH);
}

void loop() {
  key = keyScan();
  delay(100);
  if (key != '-') {
    if (key == '#') {
      if (bufPass == validPass) {
        Serial.println("\nPassed");
        digitalWrite(lock, LOW);
      } else {
        Serial.println("\nAccess Denied");
      }
      bufPass = "";
    } else if (key == '*') {
      bufPass = "";
      digitalWrite(lock, HIGH);
      Serial.println("\nLocked");
    } else {
      bufPass += key;
      Serial.print("*");
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