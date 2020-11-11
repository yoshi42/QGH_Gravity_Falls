/*///////////////////////////////////
Description:
Project for quest room - 



Also device sends a command to the contol system HC-12, that quest is done.
If something goes wrong with quest - admin can send an activating command from control panel through HC-12 interface.
Reset command resiting all to it`s start state.

21buttons - keypad
21 adress WS2811 led-segments
9buttons - keypad with leds
2 Electromagnetic locks
HC-12
DF-player mini
2 buttons OPEN|CLOSE
1 button LASTHOPE
///////////////////////////////////*/

#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h> //lib for dfplayer
#include <Keypad.h>

const byte ROWS = 3; //
const byte COLS = 7; //

const byte ROWS2 = 3; //
const byte COLS2 = 3; //

//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','4','5','6','7'},
  {'8','9','0','A','B','C','D'},
  {'E','F','G','H','I','J','K'}};

char nine_buts_Keys[ROWS2][COLS2] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'}};

byte rowPins[ROWS] = {2, 3, 4}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 6, 7, 8, 9, 10, 11}; //connect to the column pinouts of the keypad

byte rowPins2[ROWS2] = {12, 13, 14}; //connect to the row pinouts of the keypad
byte colPins2[COLS2] = {15, 16, 17}; //connect to the column pinouts of the keypad

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
Keypad customKeypad_9_but = Keypad(makeKeymap(nine_buts_Keys), rowPins2, colPins2, ROWS2, COLS2);

//led pins
#define led_1 18 
#define led_2 19
#define led_3 20
#define led_4 21
#define led_5 22
#define led_6 23
#define led_7 24
#define led_8 25
#define led_9 26
byte LED_arr[] = {led_1, led_2, led_3, led_4, led_5, led_6, led_7, led_8, led_9};

//actuator pins
#define EML 27  //Electromechanical Lock
#define LED_PWM 28  //lights
#define coin_reciever 29

//button pins
#define butt_OPEN 30
#define butt_CLOSE 31
#define butt_LASTHOPE 32

String passcode = "13256";
String secret_passcode = "87654";
int passcode_length = 5;

String temp_passcode = "";
char temp_char = '0';
char last_char = '0';

//HC_12 strings
String temp_string = ""; //variable to store information recieved form serial and compare it
String activate_snack = "act_sn#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String open_snack = "open_sn#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String reset_snack = "res_sn#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String snack_done = "sn_done#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte

bool is_passcode_win = 0;


void setup()
{
	Serial.begin(9600); //initiating serial
  DF_player.begin(9600); //initiating software serial
	mp3_set_serial (DF_player);  //set Serial for DFPlayer-mini mp3 module 
	mp3_set_volume (25);

  for(int i = 0; i <9; i++){pinMode(LED_arr[i], OUTPUT);}
  for(int i = 0; i <9; i++){digitalWrite(LED_arr[i], LOW);}

  pinMode(EML, OUTPUT);
  pinMode(LED_PWM, OUTPUT);
  digitalWrite(EML, HIGH);
  digitalWrite(LED_PWM, LOW);

  pinMode(butt_OPEN, INPUT_PULLUP);
  pinMode(butt_CLOSE, INPUT_PULLUP);
  pinMode(butt_LASTHOPE, INPUT_PULLUP);

	Serial.println("Started");
}

void loop()
{
  //HC_12_loop();  
  keypad_password_21_but();
  //check_coins();

  if(is_passcode_win == 1)
  {
    digitalWrite(EML, LOW);
    is_passcode_win = 0;
  }
}

void keypad_password_21_but()
{
  temp_char=customKeypad.getKey(); //read the keys
  
  if (last_char != temp_char)
  {
    if(temp_char != '0')
    {
      temp_passcode += temp_char;     //add to string
      Serial.println(temp_passcode);
    }
    last_char=temp_char;
  }
  temp_char = '0'; //to get available pressing 2 same numbers in a row

  if(temp_passcode.length() == passcode_length)
  {
    //check
    if(temp_passcode == passcode)
    {
      Serial.println(snack_done); ////maybe wrong syntaxis
      blinkLed();
      is_passcode_win = 1;
    }
    else if (temp_passcode == secret_passcode)
    {
      Serial.println("SECRET");
      mp3_play(5);
      delay(2000);
    }
    else
    {
      Serial.println("WRONG"); //wrong
      blinkLed_wrong();
    }
    temp_passcode = "";     //then clear the string
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

      if (temp_string == activate_snack)  //compare string with a known commands
      {
        digitalWrite(LED_PWM, HIGH);
        digitalWrite(EML, HIGH);
      }

      if (temp_string == open_snack)  //compare string with a known commands
      {
        is_passcode_win = 1;
      }

      if (temp_string == reset_snack)  //compare string with a known commands
      {
        digitalWrite(LED_PWM, LOW);
        digitalWrite(EML, HIGH);
      }
    temp_string = "";     //then clear the string
    }
  }
}