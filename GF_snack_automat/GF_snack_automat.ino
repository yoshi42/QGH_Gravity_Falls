/*///////////////////////////////////
Description:
Project for quest room - snack automat-hedden door with code panel, LED lights, electromagnetic lock, coin reciever and mp3-player

When an activating command comes from control panel, LED lights turns on.
Player should enter correct code on a code panel. When its done - lock opens! And lights becomes Pulsing for a 5 min.
If player puts a coin into a reciever - lights blinks fast 3-5 times and Stan`s voice talking.

Also device sends a command to the contol system HC-12, that quest is done.
If something goes wrong with quest - admin can send an activating command from control panel through HC-12 interface.
Reset command resiting all to it`s start state.

d0-d1 - rxtx hc-12
d2, 3, 4, 5, 6, 7 - 8 buttons in keypad mode
d8 - EML mosfet
d9 - LED PWM mosfet
d10,A7 - dfplayer rx-tx
d11,12,13,A0,1,2,3,4 - LED-outputs
A6 - coin reciever

///////////////////////////////////*/

#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h> //lib for dfplayer
#include <Keypad.h>

SoftwareSerial DF_player(A7, 10); // (Rx_pin, Tx_pin) //using a softwareSerial instead of serial, because of debugging through console and uploading a sketch

const byte ROWS = 4; //four rows
const byte COLS = 2; //four columns

//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2'},
  {'3','4'},
  {'5','6'},
  {'7','8'}};

byte rowPins[ROWS] = {2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 7}; //connect to the column pinouts of the keypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

//led pins
#define led_1 11 
#define led_2 12
#define led_3 13
#define led_4 A0
#define led_5 A1
#define led_6 A2
#define led_7 A3
#define led_8 A4

#define EML 8  //Electromechanical Lock
#define LED_PWM 9  //lights
#define coin_reciever A6

byte LED_arr[] = {led_1, led_2, led_3, led_4, led_5, led_6, led_7, led_8};

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

unsigned long time = 0;
unsigned long time1 = 0;


void setup()
{
	Serial.begin(9600); //initiating serial
  DF_player.begin(9600); //initiating software serial
	mp3_set_serial(DF_player);  //set Serial for DFPlayer-mini mp3 module 
	mp3_set_volume (25);
	mp3_play(1);

  pinMode(led_1, OUTPUT);
  pinMode(led_2, OUTPUT);
  pinMode(led_3, OUTPUT);
  pinMode(led_4, OUTPUT);
  pinMode(led_5, OUTPUT);
  pinMode(led_6, OUTPUT);
  pinMode(led_7, OUTPUT);
  pinMode(led_8, OUTPUT);
  digitalWrite(led_1, LOW);
  digitalWrite(led_2, LOW);
  digitalWrite(led_3, LOW);
  digitalWrite(led_4, LOW);
  digitalWrite(led_5, LOW);
  digitalWrite(led_6, LOW);
  digitalWrite(led_7, LOW);
  digitalWrite(led_8, LOW);

  pinMode(EML, OUTPUT);
  digitalWrite(EML, LOW);
  pinMode(LED_PWM, OUTPUT);
  digitalWrite(LED_PWM, LOW);
  pinMode(coin_reciever, INPUT_PULLUP);

	Serial.println("Started");
}

void loop()
{
  //HC_12_loop();  
  keypad_password();
  check_passcode();
  check_coins();
}

void keypad_password(){
  char pressed=customKeypad.getKey();

 if(pressed=='1'){temp_char = '1';}
 if(pressed=='2'){temp_char = '2';}
 if(pressed=='3'){temp_char = '3';}
 if(pressed=='4'){temp_char = '4';}
 if(pressed=='5'){temp_char = '5';}
 if(pressed=='6'){temp_char = '6';}
 if(pressed=='7'){temp_char = '7';}
 if(pressed=='8'){temp_char = '8';}

digitalWrite(LED_arr[pressed-1], HIGH); //turn on pressed button leds
}

void check_passcode() 
{
  if (last_char != temp_char)
  {
    if(temp_char != '0')
    {
      temp_passcode += temp_char;     //add to string
      Serial.println(temp_passcode);
    }
    last_char=temp_char;
  }
  temp_char = '0';

  if(temp_passcode.length() == passcode_length)
  {
    //check
    if(temp_passcode == passcode)
    {
      Serial.println(snack_done); ////maybe wrong syntaxis
      is_passcode_win = 1;
      blinkLed();
      mp3_play(1);
	  digitalWrite(EML, HIGH);
      delay(100);
      digitalWrite(EML, LOW);
      delay(100);
      digitalWrite(EML, HIGH);
      delay(100);
      digitalWrite(EML, LOW);
      delay(100);
      digitalWrite(EML, HIGH);
      delay(100);
      digitalWrite(EML, LOW);
      delay(3000);
      is_passcode_win = 0;
    }
    else if (temp_passcode == secret_passcode)
    {
      Serial.println("SECRET");
      mp3_play(2);
      delay(2000);
    }
    else
    {
      Serial.println("WRONG"); //wrong
      mp3_play(3);
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
        digitalWrite(EML, LOW);
      }

      if (temp_string == open_snack)  //compare string with a known commands
      {
        blinkLed();
     	mp3_play(1);
	  	digitalWrite(EML, LOW);
     	delay(3000);
      	is_passcode_win = 0;
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

void check_coins()
{
  if(analogRead(coin_reciever) >= 150)
  {
    delay(50);
    int j = random(4,9);
    mp3_play(j);

    for(int i = 0; i < 5; i++){
    digitalWrite(LED_PWM, HIGH);
    delay(200);
    digitalWrite(LED_PWM, LOW);
    delay(200);
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