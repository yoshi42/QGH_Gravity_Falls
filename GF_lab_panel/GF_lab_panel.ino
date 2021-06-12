/*///////////////////////////////////
Description:
Project for quest room - 

Рішаємо загадку 21кн, відкривається ніша з Червоною/Зеленою кнопкою (куплю)
//відкриваємо портал - активується кнопка Last Hopе, натиснули - активується загадка 9кн. 
Розгадали - відкривається ничка з підказкою, як перемогти Білла

Also device sends a command to the contol system HC-12, that quest is done.
If something goes wrong with quest - admin can send an activating command from control panel through HC-12 interface.
Reset command resiting all to it`s start state.

21buttons - keypad
21 adress WS2811 led-segments //https://alexgyver.ru/ws2812_guide/
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
#include <FastLED.h>

SoftwareSerial Serial_DF(50, 51); // (Rx_pin, Tx_pin) //using a softwareSerial instead of serial, because of debugging through console and uploading a sketch
SoftwareSerial Serial_HC(52, 53); // (Rx_pin, Tx_pin) //using a softwareSerial instead of serial, because of debugging through console and uploading a sketch

const byte ROWS = 3; //
const byte COLS = 7; //

const byte ROWS2 = 3; //
const byte COLS2 = 3; //

//define the cymbols on the buttons of the keypads
char twenty_one_buts_Keys[ROWS][COLS] = { //ascii code for 'A' = 65
  {'U','T','S','R','Q','P','O'},
  {'N','M','L','K','J','I','H'},
  {'G','F','E','D','C','B','A'}};

char nine_buts_Keys[ROWS2][COLS2] = {
  {'1','2','3'}, //ascii code for '1' = 49
  {'4','5','6'},
  {'7','8','9'}};


byte colPins[COLS] = {2, 3, 4, 5, 6, 7, 8}; //connect to the column pinouts of the keypad
byte rowPins[ROWS] = {9, 10, 11}; //connect to the row pinouts of the keypad

byte rowPins2[ROWS2] = {12, 37, 14}; //connect to the row pinouts of the keypad
byte colPins2[COLS2] = {15, 17, 16}; //connect to the column pinouts of the keypad

Keypad customKeypad_21_but = Keypad(makeKeymap(twenty_one_buts_Keys), rowPins, colPins, ROWS, COLS);
Keypad customKeypad_9_but = Keypad(makeKeymap(nine_buts_Keys), rowPins2, colPins2, ROWS2, COLS2);

//led pins
#define led_1 24 
#define led_2 25
#define led_3 26
#define led_4 21
#define led_5 22
#define led_6 23
#define led_7 18
#define led_8 19
#define led_9 20
byte LED_arr[] = {led_1, led_2, led_3, led_4, led_5, led_6, led_7, led_8, led_9};

//actuator pins
#define Just_in_case1 27  //Electromechanical Lock
#define Just_in_case2 28  //Electromechanical Lock

//button pins
#define butt_OPEN 29
#define butt_CLOSE 30
#define butt_LASTHOPE 31
//button led pins
#define butt_OPEN_LED 32
#define butt_CLOSE_LED 33
#define butt_LASTHOPE_LED 34

#define EML_doors_diary_sheet 36
#define EML_doors_open_close 39

//ws2812b strip
#define NUM_LEDS 21 //number of leds/led groups
#define DATA_PIN 35 //D0 out to pin
CRGB leds[NUM_LEDS]; // This is an array of leds.  One item for each led in your strip.

String passcode_21 = "MRDI";
String passcode_9 = "142368759";
int passcode_21_length = passcode_21.length();
int passcode_9_length = passcode_9.length();

String temp_passcode_21 = "";
char temp_char_21 = '0';
char last_char_21 = '0';
int ascii_code_21 = 0;

String temp_passcode_9 = "";
char temp_char_9 = '0';
char last_char_9 = '0';
int ascii_code_9 = 0;

//HC_12 strings
String temp_string = ""; //variable to store information recieved form serial and compare it

//Slave-Master strings
String but21_done = "but21_done#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String open_port = "open_port#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String close_port = "close_port#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String but9_done = "but9_done#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte

//Master-Slave strings
String but21_open = "but21_op#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String but9_open = "but9_op#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String reset_lab_panel = "reset_lab_panel#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte

bool is_passcode_win = 0;
bool a = 0;

int quest_pipeline = 0; ////quest logic pipeline 0->1->2->3...etc
int ppl = 0;

unsigned long t_but_9 = 0;
unsigned long t_but_9_prev = 0;
bool ledState = LOW;

unsigned long t_but_21 = 0;
unsigned long t_but_21_prev = 0;

unsigned long t_but_LH = 0;
unsigned long t_but_LH_prev = 0;

int mov1_delay = 3600000; //portal_galactic - 1hour
int mov2_delay = 20000; //О, привіт - 20sec - active, 90sec - all video
int mov3_delay = 15000; //Скільки можна чекати - 15 sec active
int mov4_delay = 15000; //Ні, ні - не робіть цього - 15 sec active
int mov5_delay = 85000; //Апокаліпсис - 85 sec active
int mov6_delay = 80000; //Фіналочка - 80 sec active

void setup()
{
	Serial.begin(9600); //initiating serial
	Serial_DF.begin(9600); //initiating software serial
	Serial_HC.begin(9600); //initiating software serial
	mp3_set_serial (Serial_DF);  //set Serial for DFPlayer-mini mp3 module 
	mp3_set_volume (25);

	//FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
	FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical

	for(int i = 0; i <9; i++){pinMode(LED_arr[i], OUTPUT);}
	for(int i = 0; i <9; i++){digitalWrite(LED_arr[i], LOW);}

	pinMode(EML_doors_diary_sheet, OUTPUT);
	pinMode(EML_doors_open_close, OUTPUT);
	pinMode(butt_OPEN_LED, OUTPUT);
	pinMode(butt_CLOSE_LED, OUTPUT);
	pinMode(butt_LASTHOPE_LED, OUTPUT);

	digitalWrite(EML_doors_diary_sheet, LOW);
	digitalWrite(EML_doors_open_close, HIGH);
	digitalWrite(butt_OPEN_LED, LOW);
	digitalWrite(butt_CLOSE_LED, LOW);
	digitalWrite(butt_LASTHOPE_LED, LOW);

	pinMode(butt_OPEN, INPUT_PULLUP);
	pinMode(butt_CLOSE, INPUT_PULLUP);
	pinMode(butt_LASTHOPE, INPUT_PULLUP);

	Serial.println("Started");
}


void loop()
{	
	HC_12_loop();
	if(quest_pipeline == 0)
	{
		keypad_password_21_but();
	}

	if(quest_pipeline == 1)
	{
		Serial.println("kp21 done, activate buttons");
		digitalWrite(EML_doors_open_close, LOW);

		digitalWrite(butt_OPEN_LED, HIGH);
		digitalWrite(butt_CLOSE_LED, HIGH);
		quest_pipeline = 11;
	}

	if(quest_pipeline == 11)
	{
		if(digitalRead(butt_CLOSE) == LOW)
		{
			delay(50);
			digitalWrite(butt_OPEN_LED, LOW);
			digitalWrite(butt_CLOSE_LED, LOW);
			quest_pipeline = 0;
			Serial_HC.println(close_port);
			Serial.println("prt clsd");

			for(int led = 0; led < NUM_LEDS; led++) {leds[led] = CRGB::Black;} //turn off all 21 leds
			delay(500);
			FastLED.show(); //refresh
			delay(10000);
		}

		if(digitalRead(butt_OPEN) == LOW)
		{	
			delay(50);
			digitalWrite(butt_OPEN_LED, LOW);
			digitalWrite(butt_CLOSE_LED, LOW);
			quest_pipeline = 2;
			Serial_HC.println(open_port);
			Serial.println("prt opnd");

			for(int led = 0; led < NUM_LEDS; led++) {leds[led] = CRGB::Black;} //turn off all 21 leds
			FastLED.show(); //refresh
			delay(5000);
		}
	}

	if(quest_pipeline == 2)
	{
		t_but_LH = millis();
	 	if(t_but_LH - t_but_LH_prev > 500) {
			t_but_LH_prev = t_but_LH;
		    if (ledState == LOW)
		      ledState = HIGH;
		    else
		      ledState = LOW;
	    	digitalWrite(butt_LASTHOPE_LED, ledState);
	  	}
	  	
	  	if(digitalRead(butt_LASTHOPE) == LOW)
	  	{	
	  		delay(50);
	  		quest_pipeline = 3;
	  		digitalWrite(butt_LASTHOPE_LED, LOW);
	  		Serial.println("lst hp prssd");
	  	}
	}

	if(quest_pipeline == 3)
	{
		kp_91_but();
	}

	if(quest_pipeline == 4)
	{
		digitalWrite(EML_doors_diary_sheet, HIGH); //open chest
		delay(500);
		digitalWrite(EML_doors_diary_sheet, LOW);
		Serial.println("all done");

		delay(10000);
		for (int i = 0; i < 9; i++){
		 	digitalWrite(LED_arr[i], LOW);}
		quest_pipeline = 0;
	}
}

void keypad_password_21_but()
{
	//read and store
	char pressed=customKeypad_21_but.getKey();
	for (int i=0; i < ROWS; i++)	{
		for(int j=0; j < COLS; j++)		{
			if(pressed == twenty_one_buts_Keys[i][j]) {temp_char_21 = pressed;
			}
		}
	}
	ascii_code_21 = temp_char_21; //chat to int

  if (last_char_21 != temp_char_21)
  {
    if(temp_char_21 != '0')
    {
      leds[ascii_code_21-65] = CRGB(255, 255, 255); //ascii code for 'A' = 65 //turn on a led# leds[0],1,2,3 etc
  	  FastLED.show(); //refresh

      temp_passcode_21 += temp_char_21;     //add to string
      Serial.println(temp_passcode_21);
    }
    last_char_21=temp_char_21;
  }
  //temp_char_21 = '0'; //to get available pressing 2 same numbers in a row

  if(temp_passcode_21.length() == passcode_21_length)
  {
    //check if right
    if(temp_passcode_21 == passcode_21)
    {
		Serial_HC.println(but21_done);
  		quest_pipeline = 1; //next quest step
  		delay(mov2_delay);
    }

    else
    {
      Serial.println("WRONG"); //wrong

      for(int led = 0; led < NUM_LEDS; led++) {leds[led] = CRGB::Black;} //turn off all leds
      delay(500);
      FastLED.show(); //refresh
    }
    temp_passcode_21 = "";     //then clear the string
  }
}

void kp_91_but()
{	
	if(temp_passcode_9.length() == 0)
	{
		t_but_9 = millis();
	 	if(t_but_9 - t_but_9_prev > 400) {
			t_but_9_prev = t_but_9;
		    if (ledState == LOW)
		      ledState = HIGH;
		    else
		      ledState = LOW;
	    	digitalWrite(LED_arr[passcode_9[0]-49], ledState); //ascii code for 0 is 49
	    }
	}

	//read and store
	char pressed=customKeypad_9_but.getKey();
	for (int i=0; i < ROWS2; i++)	{
		for(int j=0; j < COLS2; j++)		{
			if(pressed == nine_buts_Keys[i][j]) {temp_char_9 = pressed;
			}
		}
	}
	ascii_code_9 = temp_char_9; //chat to int

	if (last_char_9 != temp_char_9)
	{
		if(temp_char_9 != '0')
		{
		  temp_passcode_9 += temp_char_9;     //add to string
		  digitalWrite(LED_arr[ascii_code_9-49], HIGH); //turn on pushed button # led (get a ascii-code of button and turn it into but number in array)
		  Serial.println(temp_passcode_9);
		  //Serial.println(temp_passcode_9.length()-1);

		  if(temp_passcode_9[temp_passcode_9.length()-1] != passcode_9[temp_passcode_9.length()-1])
		  {
		  	temp_passcode_9 = "";
		  	Serial.println("WRONG"); //wrong

		  	for (int i = 0; i < 3; i++)
		  	{
		      for (int i = 0; i < 9; i++){
				digitalWrite(LED_arr[i], HIGH);}
				delay(500);
			  for (int i = 0; i < 9; i++){
			 	digitalWrite(LED_arr[i], LOW);}
				delay(500);
			}
		  }
		}
		last_char_9=temp_char_9;
	}
	//temp_char_21 = '0'; //to get available pressing 2 same numbers in a row

  if(temp_passcode_9.length() == passcode_9_length)
  {
    //check if right
    if(temp_passcode_9 == passcode_9)
    {
      Serial_HC.println(but9_done);
      quest_pipeline = 4;
    }
    else
    {
      Serial.println("WRONG"); //wrong
      for (int i = 0; i < 4; i++){ //blink all buttons
	      for (int i = 0; i < 9; i++){
			digitalWrite(LED_arr[i], HIGH);}
			delay(500);
		  for (int i = 0; i < 9; i++){
		 	digitalWrite(LED_arr[i], LOW);}
			delay(500);
		}
    }
    temp_passcode_9 = "";     //then clear the string
  }
}

void HC_12_loop()
{              //recieve something from hc-12 inerface
  while (Serial_HC.available())
  {
    char inChar = Serial_HC.read(); //store each bite in var
    temp_string += inChar;     //add to string
    //Serial.print(inChar); //Send each recieved byte back
    if (inChar == '#')       //if stop byte recieved
    {
      Serial.print(temp_string);
      Serial.println(" - copy that");

      if (temp_string == but21_open)  //compare string with a known commands
      {
      	quest_pipeline = 1;
      }
      if (temp_string == reset_lab_panel)  //compare string with a known commands
      {
      	quest_pipeline = 0;

      	digitalWrite(butt_OPEN_LED, LOW);
		digitalWrite(butt_CLOSE_LED, LOW);
		digitalWrite(butt_LASTHOPE_LED, LOW);

		for(int led = 0; led < NUM_LEDS; led++) {leds[led] = CRGB::Black;} //turn off all 21 leds
		FastLED.show(); //refresh

		for (int i = 0; i < 9; i++) {digitalWrite(LED_arr[i], LOW);}  //turn off all 9 leds
      }

      if (temp_string == but9_open)  //compare string with a known commands
      {
        quest_pipeline = 4;
      }
    temp_string = "";     //then clear the string
    }
  }
}

void test_ws2811()
{	
	for(int led = 0; led < NUM_LEDS; led++) 
	{
		leds[led] = CRGB(255, 255, 255);
		delay(500);
		FastLED.show();
	}
	for(int led = 0; led < NUM_LEDS; led++) {leds[led] = CRGB::Black;FastLED.show();}
}

/*/////////////////// TESTing ALGS /////////////////////////
void HC_12_test()
{
  if(Serial_HC.available())
  {
    char inChar = Serial_HC.read();
    Serial.print(inChar); //Send each recieved byte back
  }
}*/