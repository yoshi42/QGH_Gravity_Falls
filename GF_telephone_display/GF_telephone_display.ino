/*///////////////////////////////////
Description:
Project for quest room - Telephone plus TV with 433mhz HC-12 communication.
Device connected to phone keyboard and handset speaker. Also - it controls TV`s screen light and power button.
Also there is a ir-led that simulates the remote control signals to media pleer vith usb-disc with video

When power is supplied to the device, TV powers on but screen light still off, IR-diode simulates the remote control,
opens video from usb and put it to pause.
When user pick up a handset he hear "please, a number". If number is wrong - he hear "wrong - try again".  
When correct - "all correct - check TV", screen light is on and video plays.
Also device sends a command to the contol system HC-12, that quest is done.
If something goes wrong with quest - admin can send an activating command from control panel through HC-12 interface.
When video ends - quest is ready to call a number again.
///////////////////////////////////*/
#include <SoftwareSerial.h>
#include <Keypad.h>
#include <IRremote.h>
#include <DFPlayer_Mini_Mp3.h>

////////////HC12/////////////
SoftwareSerial HC12(4, 2); // (Rx_pin, Tx_pin) //using a softwareSerial instead of serial, because of debugging through console and uploading a sketch

String temp_string = ""; //variable to store information recieved form serial and compare it
char inChar;

//Command strings
String MCS_TV_play = "MCS_TV_play#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String cnfrm = "#SL_TV_done#"; //send command string should be "#xx...x#" format - for sure to correctly recieve a command. 1st "#" byte clears all junk before comparing, may work without it

/////////////keypad+phone/////////
int flag_numbers = 0;
int flag_true = 0;

char char1 = '1';
char char2 = '2';
char char3 = '3';
char char4 = '4';
char char5 = '5';
char char6 = '6';
char char7 = '7';
char char8 = '8';
char char9 = '9';
char char10 = '0';
char char_confirm = '#';

const byte ROWS = 4; //four rows
const byte COLS = 3; //four columns

//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}};

byte rowPins[ROWS] = {10, 9, 11, 12}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 6, 8}; //connect to the column pinouts of the keypad
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

#define activate_phone A1 //to check phone handset state - picked up/down
bool is_phone_active = false;
bool dio = false; // do it once - flag to do something just one time

/////////////////irled+TV/////////////
IRsend irsend;
const int irled = 3; //ir led transmitting commands to TV
#define screen_on A2 //turn on/off screen button relay/optrone
#define screen_LED A3 //screen led light enable pin
bool is_TV_playing = false;

void setup() 
{
  Serial.begin(9600); //initiating serial
  HC12.begin(9600); //initiating software serial

  mp3_set_serial (Serial);  //set Serial for DFPlayer-mini mp3 module 
  mp3_set_volume (15);
  
  pinMode(activate_phone, INPUT_PULLUP); //to check phone handset state - picked up/down
  pinMode(screen_LED, OUTPUT); //screen led light enable pin
  digitalWrite(screen_LED, LOW);
  pinMode(screen_on, OUTPUT); //turn on/off screen button relay/optrone
  digitalWrite(screen_on, LOW);

  Serial.println("Started");
  irblink_startup();
  Serial.println("Ready to play");
}

void loop()
{
  if(HC12.available()) 
  {
    HC_12_loop();
  }//read commands from HC12 interface

  if(digitalRead(activate_phone) == LOW && dio == false) //handset has beed picked up
  {
    delay(500);
    is_phone_active = true;
    delay(1000);
    mp3_play (1); //play audio "enter a number"

    Serial.println("handset has beed picked up");
    dio = true;
  }

  else if(digitalRead(activate_phone) == HIGH && dio == true)
  {
    delay(100);
    is_phone_active = false;
    flag_numbers = 0; //clear all pressed numbers in a phone
    flag_true = 0;

    Serial.println("handset was hung");
    dio = false;
  }

  if(is_phone_active == true) 
  {
    keypad_password();
  }
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
      if (temp_string == MCS_TV_play && is_TV_playing != true)  //compare string with a known commands
      {
        Serial.println("Admin has been activated TV");
        //HC12.println("Admin has been activated TV");
        mp3_play (3); //play audio "all correct"
        digitalWrite(screen_LED, HIGH);//Screen light on
        irsend.sendNEC(0x807F08F7, 32); //play video
        is_TV_playing = true;
        //*/delay for video length, screen light off pause again, and change is_TV_playing to false
        delay(622000); //delay 5 minutes
        digitalWrite(screen_LED, LOW); //Screen light off
        irsend.sendNEC(0x807F08F7, 32); //pause video
        is_TV_playing = false;//*/
      }
 	temp_string = "";			//then clear the string
    }
  }
}

void keypad_password(){
  char pressed=customKeypad.getKey();
  
 if(flag_true==0){if(pressed==char1){flag_true=1;}}  //1st symbol of password 
 if(flag_true==1){if(pressed==char3){flag_true=2;}}  //2nd symbol of password
 if(flag_true==2){if(pressed==char4){flag_true=3;}}  //3rd symbol of password
 if(flag_true==3){if(pressed==char4){flag_true=4;}}  //4
 if(flag_true==4){if(pressed==char8){flag_true=10;}}  //5

 if(flag_true==5){if(pressed==char6){flag_true=6;}}  //6
 if(flag_true==6){if(pressed==char7){flag_true=7;}}  //7
 if(flag_true==7){if(pressed==char8){flag_true=8;}}  //8
 if(flag_true==8){if(pressed==char9){flag_true=9;}}  //9
 if(flag_true==9){if(pressed==char10){flag_true=10;}}  //10

  if(pressed!='\0')  {
    flag_numbers++;   //to be able set a call by pressing some quantity of numbers instead of pressing # 
    Serial.println(pressed);
    mp3_play(4);
  }

 /*if(pressed==char_confirm){ */ if(flag_numbers == 5){
  mp3_play(5);
  if(flag_true==10){
    if(is_TV_playing != true) //do nothing if video is already playing
    {
      Serial.println("CORRECT PASSWORD");
      HC12.println(cnfrm);
      delay(500);
      mp3_play (3); //play audio "all correct"
      digitalWrite(screen_LED, HIGH);//Screen light on
      irsend.sendNEC(0x807F08F7, 32); //play video
      flag_numbers = 0;
      flag_true = 0;
      is_TV_playing = true;
      //*/delay for video length, screen light off pause again, and change is_TV_playing to false
      delay(622000); //delay 5 minutes
      irsend.sendNEC(0x807F08F7, 32); //pause video
      digitalWrite(screen_LED, LOW); //Screen light off
      is_TV_playing = false;//*/
    }
    else Serial.println("Video is already playing");
  }
  else if(flag_true!=10){
    Serial.println("WRONG PASSWORD");
    delay(500);
    mp3_play (2); //play audio "WRONG"
    delay(3500);
    flag_numbers = 0;
    flag_true = 0;
  }
 }
}


void irblink_startup() //TV and media pleer on
{
  /*
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
    digitalWrite(screen_on, HIGH); //push turn on button
    delay(500);
    digitalWrite(screen_on, LOW); //release turn on button
    irsend.sendNEC(0x807F807F, 32); //on
    delay(20000);
    irsend.sendNEC(0x807F6897, 32); // exit
    delay(500);
    irsend.sendNEC(0x807FDA25, 32); // menu
    delay(500);
    irsend.sendNEC(0x807F42BD, 32); //left
    delay(500);
    irsend.sendNEC(0x807F58A7, 32); //down
    delay(500);
    irsend.sendNEC(0x807FA857, 32); //ok
    delay(1000);
    irsend.sendNEC(0x807FA857, 32); //ok
    delay(1500);
    irsend.sendNEC(0x807F42BD, 32); //left
    delay(500);
    irsend.sendNEC(0x807F42BD, 32); //left
    delay(500);
    irsend.sendNEC(0x807FA857, 32); //ok
    delay(1000);
    irsend.sendNEC(0x807FA857, 32); //ok
    delay(500);
    irsend.sendNEC(0x807FE817, 32); //up
    delay(500);
    irsend.sendNEC(0x807FA857, 32); //ok
    delay(2500);
    irsend.sendNEC(0x807F08F7, 32); //pause
}
