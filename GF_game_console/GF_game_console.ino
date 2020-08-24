/*///////////////////////////////////
Description:
Project for quest room - retro game console with buttons and joystick.

When power is supplied to the device, IR-diode simulates the remote control, opens video from usb and put it to pause.
Player should enter a correct combination with a buttons, then display shows a video. 

Also device sends a command to the contol system HC-12, that quest is done.
If something goes wrong with quest - admin can send an activating command from control panel through HC-12 interface.

d0-d1 - rxtx
d2, 3, 4, d5 - 4 but + joystick in parellel connection
A4, A5, A6, A7 - 4 buttons
d6, 7, 8, 9, 10, 11, 12, 13 - 8led
A0, A1, A2 - 3 video_control
A3 - dfplayer tx (rx is empty)

///////////////////////////////////*/

#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h> //lib for dfplayer
#include <IRremote.h>

SoftwareSerial DF_player(A6, A3); // (Rx_pin, Tx_pin) //using a softwareSerial instead of serial, because of debugging through console and uploading a sketch

//button pins
#define but_left 5     //L
#define but_up A0       //U
#define but_down 2    //D Swap with D3
#define but_right 4    //R
#define but_kick A7    //K - kick
#define but_punch A5   //P - punch
#define but_kick_2 A6  //k - kick2
#define but_punch_2 A4 //p - punch2

//led pins
#define but_left_led 7
#define but_up_led 9
#define but_down_led 6
#define but_right_led 8
#define but_kick_led 10
#define but_punch_led 11
#define but_kick_2_led 12
#define but_punch_2_led 13

#define EML A1  //Electromechanical Lock

byte LED_arr[] = {but_left_led, but_up_led, but_down_led, but_right_led, but_kick_led, but_punch_led, but_kick_2_led, but_punch_2_led};

String passcode = "LLPPRLDKkPPPK";
String secret_passcode = "LLLRRRUUUDDDK";
int passcode_length = 13;

String temp_passcode = "";
char temp_char = '0';
char last_char = '0';

bool is_pressed1 = 0;
bool is_pressed2 = 0;
bool is_pressed3 = 0;
bool is_pressed4 = 0;
bool is_pressed5 = 0;
bool is_pressed6 = 0;
bool is_pressed7 = 0;
bool is_pressed8 = 0;

//HC_12 strings
String temp_string = ""; //variable to store information recieved form serial and compare it
String play_vid_1 = "play_vid_1#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String play_vid_2 = "play_vid_2#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String play_vid_3 = "play_vid_3#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String pleer_on = "pleer_on#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String pleer_off = "pleer_off#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte

bool is_console_done = false;
bool is_passcode_win = 0;

unsigned long time = 0;
unsigned long time1 = 0;

/////////////////irled/////////////
IRsend irsend;
const int irled = 3; //ir led transmitting commands to TV

void setup()
{
	Serial.begin(9600); //initiating serial
  DF_player.begin(9600); //initiating software serial
	mp3_set_serial (DF_player);  //set Serial for DFPlayer-mini mp3 module 
	mp3_set_volume (25);

  pinMode(but_left, INPUT_PULLUP);
  //pinMode(but_up, INPUT_PULLUP);  //dont need to declare as input. Physical 10k pullup
  pinMode(but_down, INPUT_PULLUP);
  pinMode(but_right, INPUT_PULLUP);
  //pinMode(but_kick, INPUT_PULLUP);  //dont need to declare as input. Physical 10k pullup
  //pinMode(but_punch, INPUT_PULLUP); //dont need to declare as input. Physical 10k pullup
  //pinMode(but_kick_2, INPUT_PULLUP);  //dont need to declare as input. Physical 10k pullup
  //pinMode(but_punch_2, INPUT_PULLUP); //dont need to declare as input. Physical 10k pullup

  pinMode(but_left_led, OUTPUT);
  pinMode(but_up_led, OUTPUT);
  pinMode(but_down_led, OUTPUT);
  pinMode(but_right_led, OUTPUT);
  pinMode(but_kick_led, OUTPUT);
  pinMode(but_punch_led, OUTPUT);
  pinMode(but_kick_2_led, OUTPUT);
  pinMode(but_punch_2_led, OUTPUT);

  pinMode(EML, OUTPUT);
  digitalWrite(EML, LOW);

  for (int i = 6; i >= 13; i++)
  {
    digitalWrite(i, LOW);
  }

	Serial.println("Started");
  irblink_startup();
  Serial.println("Ready");
}

void loop()
{  
  HC_12_loop();  
  read_buttons();
  check_passcode();

  if(is_passcode_win == 1)
  {
    irsend.sendNEC(0x807F08F7, 32); //play
    is_passcode_win = 0;
  }
}

void HC_12_test()
{
  if(Serial.available())
  {
    char inChar = Serial.read();
    Serial.print(inChar); //Send each recieved byte back
  }
}

void read_buttons()
{
  //////////////arrow buttons///////////////
  if(digitalRead(but_left) == LOW && is_pressed1 == 0){
    delay(30);
    digitalWrite(but_left_led, HIGH);
    temp_char = 'L'; 
    Serial.println("Left");
    is_pressed1 = 1;
    mp3_play(1);
    delay(100);
  }
  else if(digitalRead(but_left) == HIGH && is_pressed1 == 1){
    delay(30);
    digitalWrite(but_left_led, LOW);
    temp_char = '0';
    is_pressed1 = 0;
  }
  if(digitalRead(but_up) == LOW && is_pressed2 == 0){
    delay(30);
    digitalWrite(but_up_led, HIGH);
    temp_char = 'U';
    Serial.println("Up");
    is_pressed2 = 1;
    mp3_play(1);
    delay(100);
  }
  else if(digitalRead(but_up) == HIGH && is_pressed2 == 1){
    delay(30);
    digitalWrite(but_up_led, LOW);
    temp_char = '0';
    is_pressed2 = 0;
  }
  if(digitalRead(but_right) == LOW && is_pressed3 == 0){
    delay(30);
    digitalWrite(but_right_led, HIGH);
    temp_char = 'R';
    Serial.println("Right");
    is_pressed3 = 1;
    mp3_play(1);
    delay(100);
  }
  else if(digitalRead(but_right) == HIGH && is_pressed3 == 1){
    delay(30);
    digitalWrite(but_right_led, LOW);
    temp_char = '0';
    is_pressed3 = 0;
  }
  if(digitalRead(but_down) == LOW && is_pressed4 == 0){
    delay(30);
    digitalWrite(but_down_led, HIGH);
    temp_char = 'D';
    Serial.println("Down");
    is_pressed4 = 1;
    mp3_play(1);
    delay(100);
  }
  else if(digitalRead(but_down) == HIGH && is_pressed4 == 1){
    delay(30);
    digitalWrite(but_down_led, LOW);
    temp_char = '0';
    is_pressed4 = 0;
  }

  //////////////kick-punch buttons///////////////
  if(analogRead(but_kick) <= 100 && is_pressed5 == 0){
    delay(30);
    digitalWrite(but_kick_led, HIGH);
    temp_char = 'K';
    //Serial.println(analogRead(but_kick));
    Serial.println("Kick");
    is_pressed5 = 1;
    mp3_play(2);
    delay(100);
  }
  else if(analogRead(but_kick) > 100 && is_pressed5 == 1){
    delay(30);
    digitalWrite(but_kick_led, LOW);
    temp_char = '0';
    is_pressed5 = 0;
  }
  if(analogRead(but_punch) <= 100 && is_pressed6 == 0){
    delay(30);
    digitalWrite(but_punch_led, HIGH);
    temp_char = 'P'; 
    //Serial.println(analogRead(but_punch));
    Serial.println("Punch");
    is_pressed6 = 1;
    mp3_play(3);
    delay(100);
  }
  else if(analogRead(but_punch) > 100 && is_pressed6 == 1){
    delay(30);
    digitalWrite(but_punch_led, LOW);
    temp_char = '0';
    is_pressed6 = 0;
  }

  if(analogRead(but_kick_2) <= 100 && is_pressed7 == 0){
    delay(30);
    digitalWrite(but_kick_2_led, HIGH);
    temp_char = 'k';
    //Serial.println(analogRead(but_kick_2));
    Serial.println("kick_2");
    is_pressed7 = 1;
    mp3_play(2);
    delay(100);
  }
  else if(analogRead(but_kick_2) > 100 && is_pressed7 == 1){
    delay(30);
    digitalWrite(but_kick_2_led, LOW);
    temp_char = '0';
    is_pressed7 = 0;
  }
  if(analogRead(but_punch_2) <= 100 && is_pressed8 == 0){
    delay(30);
    digitalWrite(but_punch_2_led, HIGH);
    temp_char = 'p'; 
    //Serial.println(analogRead(but_punch_2));
    Serial.println("punch_2");
    is_pressed8 = 1;
    mp3_play(3);
    delay(100);
  }
  else if(analogRead(but_punch_2) > 100 && is_pressed8 == 1){
    delay(30);
    digitalWrite(but_punch_2_led, LOW);
    temp_char = '0';
    is_pressed8 = 0;
  }

  /*Serial.print("K = ");
  Serial.print(analogRead(but_kick));
  Serial.print("    P = ");
  Serial.println(analogRead(but_punch));*/
}

void check_passcode() 
{              //recieve something from hc-12 inerface
  if (last_char != temp_char)
  {
    if(temp_char != '0')
    {
      temp_passcode += temp_char;     //add to string
      Serial.println(temp_passcode);
    }
    last_char=temp_char;
  }
  if(temp_passcode.length() == passcode_length)
  {
    //check
    if(temp_passcode == passcode)
    {
      Serial.println("CORRECT");
      //is_console_done = 1;
      mp3_play(4);
      blinkLed();
      is_passcode_win = 1;
    }
    else if (temp_passcode == secret_passcode)
    {
      Serial.println("SECRET");
      mp3_play(6);
      delay(2000);
      digitalWrite(EML, HIGH);
      delay(500);
      digitalWrite(EML, LOW);
    }
    else
    {
      mp3_play(5);
      Serial.println("WRONG"); //wrong
      blinkLed_wrong();
    }
    temp_passcode = "";     //then clear the string
  }
}

void HC_12_loop() 
{              //recieve something from hc-12 inerface
  while (Serial.available())
  {
    char inChar = Serial.read(); //store each bite in var
    temp_string += inChar;     //add to string
    Serial.print(inChar); //Send each recieved byte back
    if (inChar == '#')       //if stop byte recieved
    {
      Serial.print(temp_string);
      Serial.println(" - copy that");
      if (temp_string == play_vid_1)  //compare string with a known commands
      {
        is_passcode_win = 1;
      }

      if (temp_string == play_vid_2)  //compare string with a known commands
      {
        irsend.sendNEC(0x807FF807, 32); //next
        delay(1000);
      }

      if (temp_string == play_vid_3)  //compare string with a known commands
      {
        irsend.sendNEC(0x807FF807, 32); //next
        delay(1000);
      }

      if (temp_string == pleer_on)  //compare string with a known commands
      {
        irblink_startup();
        delay(1000);
      }

      if (temp_string == pleer_off)  //compare string with a known commands
      {
        irsend.sendNEC(0x807F807F, 32); //off
        delay(1000);
      }
    temp_string = "";     //then clear the string
    }
  }
}

void blinkLed()
{
  for(int i = 0; i < 3; i++){
    ledHigh();
    delay(1000);
    ledLow();
    delay(1000);
  }
}

void blinkLed_wrong()
{
  for(int i = 0; i < 8; i++){
    ledHigh();
    delay(200);
    ledLow();
    delay(200);
  }
}

void ledHigh(){
  for(int i = 0; i < 8; i++)
  {
    digitalWrite(LED_arr[i], HIGH);
  }
}

void ledLow(){
  for(int i = 0; i < 8; i++)
  {
    digitalWrite(LED_arr[i], LOW);
  }
}

void irblink_startup() //Media pleer on
{
  /*
on 807F807F
exit 807F6897
menu 807FDA25
ok 807FA857
left 807F42BD
down 807F58A7
up 807FE817
play 807F08F7
next 807FF807
*/
    irsend.sendNEC(0x807F807F, 32); //on
    delay(20000);
    irsend.sendNEC(0x807FDA25, 32); // menu
    delay(700);
    irsend.sendNEC(0x807F42BD, 32); //left
    delay(700);
    irsend.sendNEC(0x807F58A7, 32); //down
    delay(700);
    irsend.sendNEC(0x807FA857, 32); //ok
    delay(3000);
    irsend.sendNEC(0x807FA857, 32); //ok
    delay(3000);
    irsend.sendNEC(0x807F08F7, 32); //play
    delay(2000);
    irsend.sendNEC(0x807F08F7, 32); //pause
}
