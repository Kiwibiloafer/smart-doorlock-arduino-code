#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include "secrets.h"

// Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;

// RFID library
#include <SPI.h>
#include <MFRC522.h>
#define SDA_PIN 5
#define RST_PIN 21
MFRC522 mfrc522(SDA_PIN, RST_PIN);
String cards[10]; // Array to store up to 10 card IDs

// Keypad library
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
String correctPIN = "";
String inputPIN = "";

bool writeCom = false;
// Pin initialization
int buttonPin = 34;
int buttonState = 0;
int relayPin = 22;
int buzzerPin = 15;
int doorSensor = 2;

// Variables for millis-based timing
unsigned long currentMillis = 0;

// Door unlock status
bool doorUnlockActive = false;
unsigned long doorUnlockStartMillis = 0;

// Buzzer single beep status
bool buzzerSingleActive = false;
unsigned long buzzerSingleStartMillis = 0;

// Buzzer tone status
bool buzzerToneActive = false;
unsigned long buzzerToneStartMillis = 0;
int buzzerToneStep = 0;

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  SPI.begin();
  mfrc522.PCD_Init();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  fetchCorrectPIN();
  fetchCard();
}

void loop() {
  currentMillis = millis();
  updateDoorUnlock();
  updateBuzzerSingle();
  updateBuzzerTone();

  fetchCommand();

  if(writeCom == true){
    activateDoorUnlock();
  }  

  
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    activateDoorUnlock();
  }

  if (digitalRead(doorSensor) == LOW) {
    activateBuzzerSingle();
    Serial.println("Door is open!");
    writeCondition(true);
    if (digitalRead(doorSensor) == HIGH) {
      activateBuzzerSingle();
      Serial.println("Door is close!");
      writeCondition(false);
    }
  }

  char key = keypad.getKey();
  if (key) {
    Serial.println(key);
    if (key == '#') {
      if (inputPIN == correctPIN) {
        activateDoorUnlock();
        Serial.println("Correct PIN! Door unlocked.");
      } else {
        Serial.println("Incorrect PIN. Try again.");
        activateBuzzerTone();
      }
      inputPIN = "";
    } else if (key == '*') {
      Serial.println("Input cleared.");
      inputPIN = "";
    } else {
      inputPIN += key;
    }
  }

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String IDTAG = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      IDTAG += mfrc522.uid.uidByte[i];
    }
    Serial.println(IDTAG);
    bool cardFound = false;
    for (String card : cards) {
      if (card == IDTAG) {
        activateDoorUnlock();
        Serial.println("Door unlocked!");
        cardFound = true;
        break;
      }
    }
    if (!cardFound) {
      activateBuzzerTone();
      Serial.println("Intruder alert: You are an intruder!");
    }
  }
}

// **FETCH FIREBASE TO OPEN DOOR**
void fetchCommand() {
  if (Firebase.RTDB.getBool(&fbdo, "Device/Write")) {
    writeCom = fbdo.boolData(); // Menyimpan nilai Device/Write ke variabel
  } else {
    Serial.println("Failed to fetch Write command: " + fbdo.errorReason());
  }
}

// **FETCH FIREBASE PIN**
void fetchCorrectPIN() {
  if (Firebase.RTDB.getString(&fbdo, "access/Pin")) {
    correctPIN = fbdo.stringData();
    Serial.println("Correct PIN updated: " + correctPIN);
  } else {
    Serial.println("Failed to fetch PIN: " + fbdo.errorReason());
  }
}


// **FETCH FIREBASE CARDS**
void fetchCard() {
  if (Firebase.RTDB.getArray(&fbdo, "access/Card")) {
    FirebaseJsonArray& jsonArray = fbdo.jsonArray();
    FirebaseJsonData jsonData; // Objek untuk menyimpan hasil data

    for (size_t i = 0; i < jsonArray.size(); i++) {
      if (jsonArray.get(jsonData, i)) { // Mengambil elemen pada indeks i
        cards[i] = jsonData.stringValue; // Simpan nilai string ke array cards
        Serial.println("Card " + String(i) + ": " + cards[i]); // Debug output
      }
    }
    Serial.println("Cards updated.");
  } else {
    Serial.println("Failed to fetch cards: " + fbdo.errorReason());
  }
}

// **WRITE STATUS DOOR**
void writeCondition(bool value) {
  if (Firebase.RTDB.setBool(&fbdo, "Device/Con", value)) {
    Serial.print("door" + value);
  }
}

// **SCENARIO FUNCTIONS**
void activateDoorUnlock() {
  doorUnlockActive = true;
  doorUnlockStartMillis = currentMillis;
  digitalWrite(buzzerPin, HIGH);
  digitalWrite(relayPin, HIGH);
  Serial.println("Door Unlocked: Access Granted!");
}

void updateDoorUnlock() {
  if (doorUnlockActive) {
    unsigned long elapsed = currentMillis - doorUnlockStartMillis;
    if (elapsed >= 1000 && elapsed < 2000) {
      digitalWrite(buzzerPin, LOW);
    } else if (elapsed >= 3000) {
      digitalWrite(relayPin, LOW);
      doorUnlockActive = false;
    }
  }
}

void activateBuzzerSingle() {
  buzzerSingleActive = true;
  buzzerSingleStartMillis = currentMillis;
  digitalWrite(buzzerPin, HIGH);
}

void updateBuzzerSingle() {
  if (buzzerSingleActive && currentMillis - buzzerSingleStartMillis >= 1000) {
    digitalWrite(buzzerPin, LOW);
    buzzerSingleActive = false;
  }
}

void activateBuzzerTone() {
  buzzerToneActive = true;
  buzzerToneStartMillis = currentMillis;
  buzzerToneStep = 0;
  Serial.println("Intruder Alert: Invalid PIN or Card Detected!");
}

void updateBuzzerTone() {
  if (buzzerToneActive) {
    unsigned long elapsed = currentMillis - buzzerToneStartMillis;
    if (buzzerToneStep == 0 && elapsed >= 500) {
      digitalWrite(buzzerPin, LOW);
      buzzerToneStep++;
      buzzerToneStartMillis = currentMillis;
    } else if (buzzerToneStep == 1 && elapsed >= 500) {
      digitalWrite(buzzerPin, HIGH);
      buzzerToneStep++;
      buzzerToneStartMillis = currentMillis;
    } else if (buzzerToneStep == 2 && elapsed >= 500) {
      digitalWrite(buzzerPin, LOW);
      buzzerToneStep++;
      buzzerToneStartMillis = currentMillis;
    } else if (buzzerToneStep == 3 && elapsed >= 500) {
      digitalWrite(buzzerPin, LOW);
      buzzerToneActive = false;
    }
  }
}
