/*///////////////////////////////////
Description:
Project for quest room - funny talking fish with color sensor TCS230 and 433mhz HC-12 communication .

Device reacts to color of light that comes from a flashlight with color crystals. When light is a right color - 
the fish begins to speak wagging its tail and opening  mouth. Then - electromagnetic lock opens.


When power is supplied to the device, TV powers on but screen light still off, IR-diode simulates the remote control,
opens video from usb and put it to pause.
When user pick up a handset he hear "please, a number". When correct - "all correct - check TV", screen light on and video plays.
Also device sends a command to the contol system HC-12, that quest is done.
If something goes wrong with quest - admin can send an activating command from control panel through HC-12 interface.

tcs230 tutorial here:
https://randomnerdtutorials.com/arduino-color-sensor-tcs230-tcs3200/

d0-d1 - rxtx
d2, 3, 4, d5 - 4 but
A6, A7 - 2 but + 2 but analog 200R
d6, 7, 8, 9, 10, 11, 12, 13 - 8led
A0, A1, A2 - 3 video_control
A3, A4 - dfplayer rxtx

///////////////////////////////////*/

#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h> //lib for dfplayer

SoftwareSerial DF_player(A6, A3); // (Rx_pin, Tx_pin) //using a softwareSerial instead of serial, because of debugging through console and uploading a sketch

//button pins
#define but_left 4     //L
#define but_up 2       //U
#define but_down 3     //D
#define but_right 5    //R
#define but_kick A7    //K - kick
#define but_punch A5   //P - punch
#define but_kick_2 A6  //k - kick2
#define but_punch_2 A4 //p - punch2

//led pins
#define but_left_led 8
#define but_up_led 6
#define but_down_led 9
#define but_right_led 7
#define but_kick_led 10
#define but_punch_led 11
#define but_kick_2_led 12
#define but_punch_2_led 13

byte LED_arr[] = {but_left_led, but_up_led, but_down_led, but_right_led, but_kick_led, but_punch_led, but_kick_2_led, but_punch_2_led};

//gpio_video_control pins
#define gpio_vid_1 A0
#define gpio_vid_2 A1
#define gpio_vid_3 A2

String passcode = "LRUDKPkp";
int passcode_length = 8;
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

bool is_console_done = false;

unsigned long time = 0;
unsigned long time1 = 0;

void setup()
{
	Serial.begin(9600); //initiating serial
  DF_player.begin(9600); //initiating software serial

	mp3_set_serial (DF_player);  //set Serial for DFPlayer-mini mp3 module 
	mp3_set_volume (25);

  pinMode(but_left, INPUT_PULLUP);
  pinMode(but_up, INPUT_PULLUP);
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

  for (int i = 6; i >= 13; i++)
  {
    digitalWrite(i, LOW);
  }

  pinMode(gpio_vid_1, OUTPUT);
  pinMode(gpio_vid_2, OUTPUT);
  pinMode(gpio_vid_3, OUTPUT);

  digitalWrite(gpio_vid_1, LOW);
  digitalWrite(gpio_vid_2, LOW);
  digitalWrite(gpio_vid_3, LOW);

	Serial.println("Started");
}

void loop()
{  
  HC_12_loop();  
  read_buttons();
  check_passcode();

  /*Serial.print("K = ");
  Serial.print(analogRead(but_kick));
  Serial.print("    P = ");
  Serial.print(analogRead(but_punch));
  Serial.print("          kick_2 = ");
  Serial.print(analogRead(but_kick_2));
  Serial.print("    punch_2 = ");
  Serial.println(analogRead(but_punch_2));
  delay(300);*/

  if(is_console_done == 1)
  {
    digitalWrite(gpio_vid_1, HIGH);
    delay(20000);
    digitalWrite(gpio_vid_1, LOW);
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
      blinkLed();
    }
    else
    {
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
        digitalWrite(gpio_vid_1, HIGH);
        delay(20000);
        digitalWrite(gpio_vid_1, LOW);
      }

      if (temp_string == play_vid_2)  //compare string with a known commands
      {
        digitalWrite(gpio_vid_2, HIGH);
        delay(20000);
        digitalWrite(gpio_vid_2, LOW);
      }

      if (temp_string == play_vid_3)  //compare string with a known commands
      {
        digitalWrite(gpio_vid_3, HIGH);
        delay(20000);
        digitalWrite(gpio_vid_3, LOW);
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