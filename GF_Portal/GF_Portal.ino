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

int last_played_mov = 0;

//Command strings
String Tele_mov1 = "Tele_mov1#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String Tele_mov2 = "Tele_mov2#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String Tele_mov3 = "Tele_mov3#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String Tele_mov4 = "Tele_mov4#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String Tele_mov5 = "Tele_mov5#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String Tele_mov6 = "Tele_mov6#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String Tele_on = "Tele_on#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte

/////////////////irled+TV/////////////
IRsend irsend;
const int irled = 3; //ir led transmitting commands to TV

void setup() 
{
  Serial.begin(9600); //initiating serial
  HC12.begin(9600); //initiating software serial
  
  pinMode(mov1, OUTPUT);
  digitalWrite(mov1, LOW);
  pinMode(mov2, OUTPUT);
  digitalWrite(mov2, LOW);
  pinMode(mov3, OUTPUT);
  digitalWrite(mov3, LOW);
  pinMode(mov4, OUTPUT);
  digitalWrite(mov4, LOW);
  pinMode(mov5, OUTPUT);
  digitalWrite(mov5, LOW);
  pinMode(mov6, OUTPUT);
  digitalWrite(mov6, LOW);

  irblink_startup();
  
  digitalWrite(mov1, HIGH); //play video 1 at start
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
        if(last_played_mov == 1)
        {
          digitalWrite(mov1, LOW);
        }
        digitalWrite(mov2, LOW);
        digitalWrite(mov3, LOW);
        digitalWrite(mov4, LOW);
        digitalWrite(mov5, LOW);
        digitalWrite(mov6, LOW);
        delay(50);
        digitalWrite(mov1, HIGH);
        last_played_mov = 1;
      }

      if(temp_string == Tele_mov2)
      { 
        if(last_played_mov == 2)
        {
          digitalWrite(mov2, LOW);
        }
        digitalWrite(mov1, LOW);
        digitalWrite(mov3, LOW);
        digitalWrite(mov4, LOW);
        digitalWrite(mov5, LOW);
        digitalWrite(mov6, LOW);
        delay(50);
        digitalWrite(mov2, HIGH);
        last_played_mov = 2;
      }

      if(temp_string == Tele_mov3)
      {
        if(last_played_mov == 3)
        {
          digitalWrite(mov3, LOW);
        }
        digitalWrite(mov1, LOW);
        digitalWrite(mov2, LOW);
        digitalWrite(mov4, LOW);
        digitalWrite(mov5, LOW);
        digitalWrite(mov6, LOW);
        delay(50);
        digitalWrite(mov3, HIGH);
        last_played_mov = 3;
      }

      if(temp_string == Tele_mov4)
      {
        if(last_played_mov == 4)
        {
          digitalWrite(mov4, LOW);
        }
        digitalWrite(mov1, LOW);
        digitalWrite(mov2, LOW);
        digitalWrite(mov3, LOW);
        digitalWrite(mov5, LOW);
        digitalWrite(mov6, LOW);
        delay(50);
        digitalWrite(mov4, HIGH);
        last_played_mov = 4;
      }

      if(temp_string == Tele_mov5)
      { 
        if(last_played_mov == 5)
        {
          digitalWrite(mov5, LOW);
        }
        digitalWrite(mov1, LOW);
        digitalWrite(mov2, LOW);
        digitalWrite(mov3, LOW);
        digitalWrite(mov4, LOW);
        digitalWrite(mov6, LOW);
        delay(50);
        digitalWrite(mov5, HIGH);
        last_played_mov = 5;
      }

      if(temp_string == Tele_mov6)
      {
        if(last_played_mov == 6)
        {
          digitalWrite(mov6, LOW);
        }
        digitalWrite(mov1, LOW);
        digitalWrite(mov2, LOW);
        digitalWrite(mov3, LOW);
        digitalWrite(mov4, LOW);
        digitalWrite(mov5, LOW);
        delay(50);
        digitalWrite(mov6, HIGH);
        last_played_mov = 6;
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
    irsend.sendNEC(0x5FAFA05, 32); //on TV
    delay(1000);
}
