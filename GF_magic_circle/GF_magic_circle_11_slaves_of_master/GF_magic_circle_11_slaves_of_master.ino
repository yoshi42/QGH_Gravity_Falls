/*///////////////////////////////////
Description:
Project for quest room - magic circle that identifying 12 signs that should be putted on right places. Using arduino nano with atmega 328P.

Device read 12 RFID cards integrated into plywood signs with 12 RC522 RFID readera and connected to master control system with 433mhz HC-12 interface.
Also device sends a command to the contol system HC-12, that quest is done.
///////////////////////////////////*/

#include <SPI.h>
#include <MFRC522.h>

//Standart SPI Arduino pro mini pin layout
//RST/Reset   RST     D10
//SPI SS      SDA(SS) D9
//SPI MOSI    MOSI    D11
//SPI MISO    MISO    D12
//SPI SCK     SCK     D13 
#define RST_PIN          10
#define SS_0_PIN         9
#define done_pin         8
#define card_present_pin 7

const int NR_OF_READERS = 1;

byte ssPins[] = {SS_0_PIN};

MFRC522 mfrc522[NR_OF_READERS];   // Create MFRC522 instance.
unsigned long bufCard = 0;
unsigned long masCard [12] = {
  0xB6BE7513, //0
  0xF67D23C6, //1
  0x769E36C6, //2
  0x564F1A6B, //3
  0xD6C8EDAD, //4
  0x1644CE12, //5
  0xB62234AE, //6
  0x648F2AD , //7
  0xE6F54AAF, //8
  0x2610FDAC, //9
  0xC68D01AE, //10
  0xF676FAAD //11
};
byte nCard = 0;

void setup()
{

  Serial.begin(9600); // Initialize serial communications with the PC
  SPI.begin();        // Init SPI bus

  pinMode(done_pin, OUTPUT);
  digitalWrite(done_pin, HIGH);
  pinMode(card_present_pin, OUTPUT);
  digitalWrite(card_present_pin, HIGH);

  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN); // Init each MFRC522 card
    Serial.print(F("Reader "));
    Serial.print(reader);
    Serial.print(F(": "));
    mfrc522[reader].PCD_DumpVersionToSerial();
  }
}

/**
   Main loop.
*/
void loop() {

  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    // Look for new cards

    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial()) {
      Serial.print(F("Reader "));
      Serial.print(reader);
      nCard = reader;
      // Show some details of the PICC (that is: the tag/card)
      Serial.print(F(": Card UID:"));
      dump_byte_array(mfrc522[reader].uid.uidByte, mfrc522[reader].uid.size);

      //Serial.print(" ");
      //Serial.println();
      // Halt PICC
      mfrc522[reader].PICC_HaltA();
      // Stop encryption on PCD
      mfrc522[reader].PCD_StopCrypto1();
    }
  }
  if (mfrc522[0].PICC_IsNew
    CardPresent()){
    digitalWrite(card_present_pin, LOW);
  }
  else {digitalWrite(card_present_pin, HIGH);}
}

void dump_byte_array(byte *buffer, byte bufferSize)
{
  for (byte i = 0; i < bufferSize; i++)
  {
    //Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    //Serial.print(buffer[i], HEX);
    //Serial.print("  ");
    bufCard = bufCard << 8;
    bufCard = bufCard + buffer[i];
  }
  Serial.println(bufCard, HEX);

  if (bufCard == masCard[1])
  {
    Serial.println("card1");
    digitalWrite(done_pin, LOW);
  }
  
  else if(bufCard != masCard[1])
  {
    digitalWrite(done_pin, HIGH);
    Serial.println("card_another");
  }
  bufCard = 0;
}
