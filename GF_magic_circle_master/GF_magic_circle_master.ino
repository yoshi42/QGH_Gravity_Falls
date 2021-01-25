/*///////////////////////////////////
Description:
Project for quest room - magic circle that identifying 12 signs that should be putted on right places. Using arduino nano with atmega 328P.

Device read 12 RFID cards integrated into plywood signs with 10 RC522 RFID readera and connected to master control system with 433mhz HC-12 interface.
Also device sends a command to the contol system HC-12, that quest is done.
///////////////////////////////////*/

#include <SoftwareSerial.h>
#include <FastLED.h>

SoftwareSerial Serial_HC(A4, A5); // (Rx_pin, Tx_pin) //using a softwareSerial instead of serial, because of debugging through console and uploading a sketch

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

#define readers_rst 11 //LOW for a sec is reset
#define bipolarpnp 13


//ws2812b strip
#define NUM_LEDS 10 //number of leds/led groups
#define DATA_PIN 10 //D0 out to pin
CRGB leds[NUM_LEDS]; // This is an array of leds.  One item for each led in your strip.


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

//HC_12 strings
String temp_string = ""; //variable to store information recieved form serial and compare it

//Slave-Master strings
String mgc_crcl_done = "mgc_crcl_done#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte

void setup()
{
  Serial.begin(9600);
  Serial_HC.begin(9600); //initiating software serial

  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical

  for (int reader = 0; reader < NR_OF_READERS; reader++) {
    pinMode(ssPins[reader], INPUT_PULLUP);
  }

  pinMode(readers_rst, OUTPUT);
  digitalWrite(readers_rst, HIGH);
  delay(1000);
  digitalWrite(readers_rst, LOW);

}

void loop ()
{ 
  for (int reader = 0; reader < NR_OF_READERS; reader++) 
  {
    if (digitalRead(ssPins[reader]) == reader_ok_state)
    {
      delay(20);
      leds[reader] = CRGB(0, 70, 0); //turn on led
      FastLED.show(); //refresh
    }

    if (digitalRead(ssPins[reader]) != reader_ok_state)
    {
      delay(20);
      leds[reader] = CRGB::Black; //turn off led
      FastLED.show(); //refresh
    }
  }

  if ((digitalRead(ssPins[0]) == reader_ok_state &&
      digitalRead(ssPins[1]) == reader_ok_state &&
      digitalRead(ssPins[2]) == reader_ok_state &&
      digitalRead(ssPins[3]) == reader_ok_state &&
      digitalRead(ssPins[4]) == reader_ok_state &&
      digitalRead(ssPins[5]) == reader_ok_state &&
      digitalRead(ssPins[6]) == reader_ok_state &&
      digitalRead(ssPins[7]) == reader_ok_state &&
      digitalRead(ssPins[8]) == reader_ok_state &&
      digitalRead(ssPins[9]) == reader_ok_state) &&
      trigger_logic_state == false) 
  {
    delay(200);
    Serial.println("all readers are ok");
    Serial_HC.println(mgc_crcl_done);

    trigger_logic_state = true;
    trigger_logic_state2 = true;

    for (int reader = 0; reader < NR_OF_READERS; reader++) 
    {
      leds[reader] = CRGB(70, 0, 0); //turn on led
      FastLED.show(); //refresh
    }
    delay(5000);

    digitalWrite(readers_rst, HIGH);
    delay(500);
    digitalWrite(readers_rst, LOW);
    Serial.println("resetting readers");
  }

  if((digitalRead(ssPins[0]) == !reader_ok_state ||
      digitalRead(ssPins[1]) == !reader_ok_state ||
      digitalRead(ssPins[2]) == !reader_ok_state ||
      digitalRead(ssPins[3]) == !reader_ok_state ||
      digitalRead(ssPins[4]) == !reader_ok_state ||
      digitalRead(ssPins[5]) == !reader_ok_state ||
      digitalRead(ssPins[6]) == !reader_ok_state ||
      digitalRead(ssPins[7]) == !reader_ok_state ||
      digitalRead(ssPins[8]) == !reader_ok_state ||
      digitalRead(ssPins[9]) == !reader_ok_state) &&
      trigger_logic_state2 == true)
  {
    delay(200);
    Serial.println("NOT");
    trigger_logic_state = false;
    trigger_logic_state2 = false;
  }

}