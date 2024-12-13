#include <SPI.h>
#include <MFRC522.h>

#define SDA_PIN 5
#define RST_PIN 21

MFRC522 mfrc522(SDA_PIN, RST_PIN);


void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("dekatkan Kartu Anda Ke RFID Reader");
  Serial.println();
}

void loop() {
  if(! mfrc522.PICC_IsNewCardPresent())
    return ;
  
  if(! mfrc522.PICC_ReadCardSerial())
    return ;
  
  String IDTAG = "";
  for(byte i=0; i<mfrc522.uid.size; i++){
    IDTAG += mfrc522.uid.uidByte[i];
  }
  Serial.println(IDTAG);
}