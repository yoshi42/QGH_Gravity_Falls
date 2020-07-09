/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read data from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * Example sketch/program showing how to read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the ID/UID, type and any data blocks it can read. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * If your reader supports it, this sketch/program will read all the PICCs presented (that is: multiple tag reading).
 * So if you stack two or more PICCs on top of each other and present them to the reader, it will first output all
 * details of the first and then the next PICC. Note that this may take some time as all data blocks are dumped, so
 * keep the PICCs at reading distance until complete.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
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

#define RST_PIN         10          // Configurable, see typical pin layout above

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

MFRC522 mfrc522_0(SS_0_PIN, RST_PIN);  // Create MFRC522 instance
MFRC522 mfrc522_1(SS_5_PIN, RST_PIN);  // Create MFRC522 instance
unsigned long bufCard = 0;

void setup() 
{
  Serial.begin(9600);   // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();      // Init SPI bus
  mfrc522_0.PCD_Init();   // Init MFRC522
  mfrc522_0.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  delay(100);
  mfrc522_1.PCD_Init();   // Init MFRC522
  mfrc522_1.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
}

void loop() {
  if (mfrc522_0.PICC_IsNewCardPresent() && mfrc522_0.PICC_ReadCardSerial()) {
      //Serial.print(F("Reader "));
      //Serial.print(reader);
      //nCard = reader;
      // Show some details of the PICC (that is: the tag/card)
      Serial.print(F("Card UID:"));
      dump_byte_array(mfrc522_0.uid.uidByte, mfrc522_0.uid.size);

      //Serial.print(" ");
      //Serial.println();
      // Halt PICC
      mfrc522_0.PICC_HaltA();
      // Stop encryption on PCD
      mfrc522_0.PCD_StopCrypto1();
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
}