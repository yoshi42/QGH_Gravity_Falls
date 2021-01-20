/*///////////////////////////////////
Description:
Project for quest room - magic circle that identifying 12 signs that should be putted on right places. Using arduino nano with atmega 328P.

Device read 12 RFID cards integrated into plywood signs with 10 RC522 RFID readera and connected to master control system with 433mhz HC-12 interface.
Also device sends a command to the contol system HC-12, that quest is done.
///////////////////////////////////*/

#include <SoftwareSerial.h>

#define SS_0_PIN        3
#define SS_1_PIN        9
#define SS_2_PIN        2
#define SS_3_PIN        8
#define SS_4_PIN        A0
#define SS_5_PIN        7
#define SS_6_PIN        A1
#define SS_7_PIN        6
#define SS_8_PIN        A2
#define SS_9_PIN        5
#define SS_10_PIN       A3
#define SS_11_PIN       4

#define is_card_present 11

const int NR_OF_READERS = 12;

byte ssPins[] = {SS_0_PIN, SS_1_PIN, SS_2_PIN, SS_3_PIN, SS_4_PIN, SS_5_PIN,
                 SS_6_PIN, SS_7_PIN, SS_8_PIN, SS_9_PIN, SS_10_PIN, SS_11_PIN};

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

int num_of_ok = 0;
bool reader_ok_state = false;

bool trigger_logic_state = false;
bool trigger_logic_state2 = false;

#define ledpin_ws2812 10;

void setup()
{
  Serial.begin(9600);

  for (int reader = 0; reader < NR_OF_READERS; reader++) {
    pinMode(ssPins[reader], INPUT_PULLUP);
  }
}

void loop ()
{
  if (digitalRead(ssPins[1]) == reader_ok_state &&
      /*digitalRead(ssPins[1]) == reader_ok_state &&
      digitalRead(ssPins[2]) == reader_ok_state &&
      digitalRead(ssPins[3]) == reader_ok_state &&
      digitalRead(ssPins[4]) == reader_ok_state &&
      digitalRead(ssPins[5]) == reader_ok_state &&
      digitalRead(ssPins[6]) == reader_ok_state &&
      digitalRead(ssPins[7]) == reader_ok_state &&
      digitalRead(ssPins[8]) == reader_ok_state &&
      digitalRead(ssPins[9]) == reader_ok_state &&*/
      trigger_logic_state == false) 
  {
    delay(200);
    Serial.println("all readers are ok");
    trigger_logic_state = true;
    trigger_logic_state2 = true;
  }

  if((digitalRead(ssPins[1]) == !reader_ok_state //||
      /*digitalRead(ssPins[1]) == !reader_ok_state ||
      digitalRead(ssPins[2]) == !reader_ok_state ||
      digitalRead(ssPins[3]) == !reader_ok_state ||
      digitalRead(ssPins[4]) == !reader_ok_state ||
      digitalRead(ssPins[5]) == !reader_ok_state ||
      digitalRead(ssPins[6]) == !reader_ok_state ||
      digitalRead(ssPins[7]) == !reader_ok_state ||
      digitalRead(ssPins[8]) == !reader_ok_state ||
      digitalRead(ssPins[9]) == !reader_ok_state)*/ &&
      trigger_logic_state2 == true))
  {
    delay(200);
    Serial.println("NOT");
    trigger_logic_state = false;
    trigger_logic_state2 = false;
  }

}