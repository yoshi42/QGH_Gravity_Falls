/*///////////////////////////////////
Description:
Project for quest room - TV with RPi video control and HC-12 communication.

When power is supplied to the device, TV powers on by a command from
Master Control System sends an activating command for playing each video through HC-12 interface.
When video ends - it ready to play next.
///////////////////////////////////*/
#include <SoftwareSerial.h>
#include <IRremote.h>

////////////HC12/////////////
SoftwareSerial HC12(6, 5); // (Rx_pin, Tx_pin) //using a softwareSerial instead of serial, because of debugging through console and uploading a sketch

String temp_string = ""; //variable to store information recieved form serial and compare it
char inChar;


#define mov1 7 //to check phone handset state - picked up/down
#define mov2 8 //to check phone handset state - picked up/down
#define mov3 9 //to check phone handset state - picked up/down
#define mov4 10 //to check phone handset state - picked up/down
#define mov5 11 //to check phone handset state - picked up/down
#define mov6 12 //to check phone handset state - picked up/down

int delay_mov1 = 100000; //milisec
int delay_mov2 = 100000; //milisec
int delay_mov3 = 100000; //milisec
int delay_mov4 = 100000; //milisec
int delay_mov5 = 100000; //milisec
int delay_mov6 = 100000; //milisec

unsigned long t_mov1 = 0;
unsigned long t_mov1_prev = 0;
unsigned long t_mov2 = 0;
unsigned long t_mov2_prev = 0;
unsigned long t_mov3 = 0;
unsigned long t_mov3_prev = 0;
unsigned long t_mov4 = 0;
unsigned long t_mov4_prev = 0;
unsigned long t_mov5 = 0;
unsigned long t_mov5_prev = 0;
unsigned long t_mov6 = 0;
unsigned long t_mov6_prev = 0;

//Command strings
String Tele_mov1 = "Tele_mov1#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String Tele_mov2 = "Tele_mov2#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String Tele_mov3 = "Tele_mov3#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String Tele_mov4 = "Tele_mov4#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String Tele_mov5 = "Tele_mov5#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String Tele_mov6 = "Tele_mov6#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String Tele_on = "Tele_on#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte

bool dio = false; // do it once - flag to do something just one time

/////////////////irled+TV/////////////
IRsend irsend;
const int irled = 3; //ir led transmitting commands to TV

void setup() 
{
  Serial.begin(9600); //initiating serial
  HC12.begin(9600); //initiating software serial
  
  pinMode(mov1, OUTPUT);
  digitalWrite(mov1, HIGH);
  pinMode(mov2, OUTPUT);
  digitalWrite(mov2, HIGH);
  pinMode(mov3, OUTPUT);
  digitalWrite(mov3, HIGH);
  pinMode(mov4, OUTPUT);
  digitalWrite(mov4, HIGH);
  pinMode(mov5, OUTPUT);
  digitalWrite(mov5, HIGH);
  pinMode(mov6, OUTPUT);
  digitalWrite(mov6, HIGH);

  delay(2000); //wait for TV became to react an ir-comands
  irblink_startup();
}

void loop()
{
  HC_12_loop();
}

void HC_12_loop() 
{              //recieve something from hc-12 inerface
  while (HC12.available())
  {
    char inChar = HC12.read(); //store each bite in var
    temp_string += inChar;     //add to string
    if (inChar == '#')		   //if stop byte recieved
    {
      Serial.print(temp_string);
      Serial.println(" - copy that");

      if(temp_string == Tele_mov1)
      { 
        digitalWrite(mov1, LOW);
        t_mov1 = millis();
        if(t_mov1 - t_mov1_prev > delay_mov1) 
        {
          t_mov1_prev = t_mov1;
          digitalWrite(mov1, HIGH);
        }
      }

      if(temp_string == Tele_mov2)
      { 
        digitalWrite(mov2, LOW);
        t_mov2 = millis();
        if(t_mov2 - t_mov2_prev > delay_mov2) 
        {
          t_mov2_prev = t_mov2;
          digitalWrite(mov2, HIGH);
        }
      }

      if(temp_string == Tele_mov3)
      {
        digitalWrite(mov3, LOW);
        t_mov3 = millis();
        if(t_mov3 - t_mov3_prev > delay_mov3) 
        {
          t_mov3_prev = t_mov3;
          digitalWrite(mov3, HIGH);
        }
      }

      if(temp_string == Tele_mov4)
      {
        digitalWrite(mov4, LOW);
        t_mov4 = millis();
        if(t_mov4 - t_mov4_prev > delay_mov4) 
        {
          t_mov4_prev = t_mov4;
          digitalWrite(mov4, HIGH);
        }
      }

      if(temp_string == Tele_mov5)
      { 
        digitalWrite(mov5, LOW);
        t_mov5 = millis();
        if(t_mov5 - t_mov5_prev > delay_mov5) 
        {
          t_mov5_prev = t_mov5;
          digitalWrite(mov5, HIGH);
        }
      }

      if(temp_string == Tele_mov6)
      {
        digitalWrite(mov6, LOW);
        t_mov6 = millis();
        if(t_mov6 - t_mov6_prev > delay_mov6) 
        {
          t_mov6_prev = t_mov6;
          digitalWrite(mov6, HIGH);
        }
      }

      if (temp_string == Tele_on)  //compare string with a known commands
      {
        irblink_startup();
      }


 	    temp_string = "";			//then clear the string
    }
  }
}

void irblink_startup() //TV and media pleer on
{
  /*TV ELENBERG
on 807F807F
delay 5-10 sec
exit 807F6897
menu 807FDA25
left 807F42BD
down 807F58A7
ok 807FA857
ok 807FA857
left 807F42BD
left 807F42BD
ok 807FA857
ok 807FA857
up 807FE817
ok 807FA857
pause 807F08F7
play 807F08F7
*/
    irsend.sendNEC(0x807F807F, 32); //on
}
