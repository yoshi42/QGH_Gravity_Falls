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

String b1 = "b1#"; //Портал
String b2 = "b2#"; //О, привіт
String b3 = "b3#"; //Скільки можна чекати
String b4 = "b4#"; //Не робіть цього
String b5 = "b5#"; //Які ж ви довірливі
String b6 = "b6#"; //Фіналочка
String b7 = "b7#"; //Закрити відео
String re = "re#"; //Reboot RPi

//Slave-Master - command tells that video is playing
String Vid_play = "Vid_play#";

/////////////////irled+TV/////////////
IRsend irsend;
const int irled = 3; //ir led transmitting commands to TV

void setup() 
{
  Serial.begin(9600); //initiating serial
  HC12.begin(9600); //initiating software serial

  irblink_startup();
}

void loop()
{
  HC_12_loop();
  //test_vid();
}

void HC_12_loop() 
{              //recieve something from hc-12 inerface
  while (HC12.available())
  {
    char inChar = HC12.read(); //store each bite in var
    temp_string += inChar;     //add to string
    if (inChar == '#')		   //if stop byte recieved
    {
      //Serial.print(temp_string);
      //Serial.println(" - copy that");

      if(temp_string == Tele_mov1){Serial.print(b1);delay(200);HC12.print(Vid_play)}
      if(temp_string == Tele_mov2){Serial.print(b2);delay(200);HC12.print(Vid_play)}
      if(temp_string == Tele_mov3){Serial.print(b3);delay(200);HC12.print(Vid_play)}
      if(temp_string == Tele_mov4){Serial.print(b4);delay(200);HC12.print(Vid_play)}
      if(temp_string == Tele_mov5){Serial.print(b5);delay(200);HC12.print(Vid_play)}
      if(temp_string == Tele_mov6){Serial.print(b6);delay(200);HC12.print(Vid_play)}

      if(temp_string == b1){Serial.print(b1);delay(200);HC12.print(Vid_play)}
      if(temp_string == b2){Serial.print(b2);delay(200);HC12.print(Vid_play)}
      if(temp_string == b3){Serial.print(b3);delay(200);HC12.print(Vid_play)}
      if(temp_string == b4){Serial.print(b4);delay(200);HC12.print(Vid_play)}
      if(temp_string == b5){Serial.print(b5);delay(200);HC12.print(Vid_play)}
      if(temp_string == b6){Serial.print(b6);delay(200);HC12.print(Vid_play)}
      if(temp_string == b7){Serial.print(b7);delay(200);}

      if(temp_string == re){Serial.print(re);delay(200);}
      if(temp_string == Tele_on){irblink_startup();}

 	    temp_string = "";			//then clear the string
    }
  }
}

void serial_loop() 
{              //recieve something from hc-12 inerface
  while (Serial.available())
  {
    char inChar = Serial.read(); //store each bite in var
    Serial.print(inChar);
  }
}

void test_vid()
{
  delay(15000);
  Serial.print(b1);
  delay(10000);
  Serial.print(b2);
  delay(20000);
  Serial.print(b3);
  delay(15000);
  Serial.print(b4);
  delay(15000);
  Serial.print(b5);
  delay(85000);
  Serial.print(b6);
  delay(80000);
  Serial.print(b7);
}

void irblink_startup() //TV and media pleer on
{ 
    delay(10000);
    irsend.sendNEC(0x4FB4AB5, 32); //on TV
    delay(1000);
}
