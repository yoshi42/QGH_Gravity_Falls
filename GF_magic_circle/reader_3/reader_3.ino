#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         10          // Configurable, see typical pin layout above
#define SS_1_PIN        9         // Configurable, take a unused pin, only HIGH/LOW required, must be diffrent to SS 2
#define SS_2_PIN        8          // Configurable, take a unused pin, only HIGH/LOW required, must be diffrent to SS 1
#define SS_3_PIN        7          // Configurable, take a unused pin, only HIGH/LOW required, must be diffrent to SS 1
#define OUT_1 2
#define OUT_2 3
#define OUT_3 4
#define NR_OF_READERS   3

byte ssPins[] = {SS_1_PIN, SS_2_PIN, SS_3_PIN};

MFRC522 mfrc522[NR_OF_READERS];   // Create MFRC522 instance.
unsigned long bufCard = 0;
unsigned long masCard [6] = {
  0x564F1A6B, //1
  0xA08D6C19,
  0x79A5B55A,
  0x071EBE46,
  0xB989C300,
  0xE7959F4D
};
byte nCard = 0;
void setup()
{

  Serial.begin(9600); // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  pinMode(OUT_1, OUTPUT);
  pinMode(OUT_2, OUTPUT);
  pinMode(OUT_3, OUTPUT);
  digitalWrite(OUT_1, LOW);
  digitalWrite(OUT_2, LOW);
  digitalWrite(OUT_3, LOW);

  SPI.begin();        // Init SPI bus

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

      Serial.print(" ");
      Serial.println();
      // Halt PICC
      mfrc522[reader].PICC_HaltA();
      // Stop encryption on PCD
      mfrc522[reader].PCD_StopCrypto1();
    }
  }
}

void dump_byte_array(byte *buffer, byte bufferSize)
{
  for (byte i = 0; i < bufferSize; i++)
  {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
    Serial.print("  ");
    bufCard = bufCard << 8;
    bufCard = bufCard + buffer[i];
  }
  Serial.print(bufCard, HEX);

  if ((bufCard == masCard[0]) || (bufCard == masCard[3]))
  {
    if (nCard == 0)
    {
      Serial.println("card1");
      digitalWrite(OUT_1, HIGH);
    }
  }
  if ((bufCard == masCard[1]) || (bufCard == masCard[4]))
  {
    if (nCard == 1)
    {
      digitalWrite(OUT_2, HIGH);
    }
  }
  if ((bufCard == masCard[2]) || (bufCard == masCard[5]))
  {
    if (nCard == 2)
    {
      digitalWrite(OUT_3, HIGH);
    }
  }
  bufCard = 0;
}
