/*///////////////////////////////////
Description:
Project for quest room - magic circle that identifying 12 signs that should be putted on right places. Using arduino nano with atmega 328P.

Device read 12 RFID cards integrated into plywood signs with 12 RC522 RFID readera and connected to master control system with 433mhz HC-12 interface.
Also device sends a command to the contol system HC-12, that quest is done.
///////////////////////////////////*/

/* RC522 library sample pinout description

Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>

//Standart SPI Arduino nano pin layout
//RST/Reset   RST     D10
//SPI SS      SDA(SS) D9
//SPI MOSI    MOSI    D11
//SPI MISO    MISO    D12
//SPI SCK     SCK     D13 
#define RST_PIN         10        // Configurable, see typical pin layout above
// Activating a readers can be D0-A5 OUTPUT pins
#define SS_0_PIN        8
#define SS_1_PIN        9
#define SS_2_PIN        7
#define SS_3_PIN        6
#define SS_4_PIN        5
#define SS_5_PIN        4
#define SS_6_PIN        3
#define SS_7_PIN        2
#define SS_8_PIN        A0
#define SS_9_PIN        A1
#define SS_10_PIN       A2
#define SS_11_PIN       A3

const int NR_OF_READERS = 12;

byte ssPins[] = {SS_0_PIN, SS_1_PIN, SS_2_PIN, SS_3_PIN, SS_4_PIN, SS_5_PIN, SS_6_PIN, SS_7_PIN, SS_8_PIN, SS_9_PIN, SS_10_PIN, SS_11_PIN};

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

  if (bufCard == masCard[0])
  {
    if (nCard == 0)
    {
      Serial.println("card0");
    }
  }
  if (bufCard == masCard[1])
  {
    if (nCard == 1)
    {
      Serial.println("card1");
    }
  }
  if (bufCard == masCard[2])
  {
    if (nCard == 2)
    {
      Serial.println("card2");
    }
  }
  bufCard = 0;
}
