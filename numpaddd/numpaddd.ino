#include <Keypad.h>

const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {32, 33, 25, 26}; 
byte colPins[COLS] = {27, 14, 12, 13}; 

Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
const String correctPIN = "1234";
String inputPIN = "";

int buttonPin = 34;
int buttonState = 0;

void setup(){
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);
  Serial.println("restart");
}
  
void loop(){
  char key = keypad.getKey();
  if (key) {
    Serial.println(key);

    if (key == '#') {
      // Check if entered PIN is correct
      if (inputPIN == correctPIN) {
        Serial.println("Correct PIN! Door unlocked.");
      } else {
        Serial.println("Incorrect PIN. Try again.");
      }
      inputPIN = ""; // Reset input
    } else if (key == '*') {
      // Reset input
      Serial.println("Input cleared.");
      inputPIN = "";
    } else {
      // Append key to inputPIN
      inputPIN += key;
    }
  }
  buttonState = digitalRead(buttonPin);
  if ( buttonState == HIGH) 
  {
    Serial.println("door unlocked");
    delay(1000);
  }
}