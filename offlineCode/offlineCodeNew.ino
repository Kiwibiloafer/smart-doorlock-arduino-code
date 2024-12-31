#include <Keypad.h>
#include <SPI.h>
#include <MFRC522.h>

#define SDA_PIN 5
#define RST_PIN 21

MFRC522 mfrc522(SDA_PIN, RST_PIN);
String card1 = "53155164172";
String card2 = "131226848";

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
int relayPin = 35;
int buzzerPin = 15;
int doorSensor = 2;

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("dekatkan Kartu Anda Ke RFID Reader");
  Serial.println();
}

void checkButton() {
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    Serial.println("door unlocked");
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(relayPin, HIGH);
    delay(1000);
    digitalWrite(buzzerPin, LOW);
    digitalWrite(relayPin, LOW);
  }
}

void checkDoorSensor() {
  if (digitalRead(doorSensor) == HIGH) {
    digitalWrite(buzzerPin, HIGH);
    delay(1000);
    digitalWrite(buzzerPin, LOW);
    delay(1000);
  }
}

void handleKeypadInput() {
  char key = keypad.getKey();
  if (key) {
    Serial.println(key);

    if (key == '#') {
      if (inputPIN == correctPIN) {
        Serial.println("Correct PIN! Door unlocked.");
        digitalWrite(buzzerPin, HIGH);
        digitalWrite(relayPin, HIGH);
        delay(1000);
        digitalWrite(buzzerPin, LOW);
        digitalWrite(relayPin, LOW);
      } else {
        Serial.println("Incorrect PIN. Try again.");
        digitalWrite(buzzerPin, HIGH);
        delay(500);
        digitalWrite(buzzerPin, LOW);
        delay(500);
        digitalWrite(buzzerPin, HIGH);
        delay(500);
        digitalWrite(buzzerPin, LOW);
      }
      inputPIN = "";
    } else if (key == '*') {
      Serial.println("Input cleared.");
      inputPIN = "";
    } else {
      inputPIN += key;
    }
  }
}

void handleRFID() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String IDTAG = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      IDTAG += mfrc522.uid.uidByte[i];
    }
    Serial.println(IDTAG);
    if (IDTAG == card1) {
      Serial.println("door unlocked");
      digitalWrite(buzzerPin, HIGH);
      digitalWrite(relayPin, HIGH);
      delay(1000);
      digitalWrite(buzzerPin, LOW);
      digitalWrite(relayPin, LOW);
    } else {
      Serial.println("you are intruder");
      digitalWrite(buzzerPin, HIGH);
      delay(500);
      digitalWrite(buzzerPin, LOW);
      delay(500);
      digitalWrite(buzzerPin, HIGH);
      delay(500);
      digitalWrite(buzzerPin, LOW);
    }
  }
}

void loop() {
  checkButton();
  checkDoorSensor();
  handleKeypadInput();
  handleRFID();
}
