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
///////////////////////////////////*/

#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h> //lib for dfplayer

SoftwareSerial HC12(9, 10); // (Rx_pin, Tx_pin) //using a softwareSerial instead of serial, because of debugging through console and uploading a sketch
SoftwareSerial DF_player(4, 2); // (Rx_pin, Tx_pin) //using a softwareSerial instead of serial, because of debugging through console and uploading a sketch

String temp_string = ""; //variable to store information recieved form serial and compare it

//Command strings
String MCS_karp_play = "MCS_karp_play#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String cnfrm = "#SL_karp_done#"; //send command string should be "#xx...x#" format - for sure to correctly recieve a command. 1st "#" byte clears all junk before comparing, may work without it

//////////COLOR SENSOR/////////////

// Pin definitions
#define S0 11
#define S1 7
#define S2 6
#define S3 12
#define OE 8  // LOW = ENABLED 
#define OUT 13

// Stores frequency read by the photodiodes
int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;
int whiteFrequency = 0;

char what_color = '0';

//actuator definitions
#define tail 5 //fish tail
#define mouth 3 //fish mouth
#define EML_Table A0 //Table - magnetic lock

bool is_fish_done = false;
unsigned  long time = 0;

void setup()
{
	Serial.begin(9600); //initiating serial
  DF_player.begin(9600); //initiating software serial
	HC12.begin(9600); //initiating software serial

	mp3_set_serial (DF_player);  //set Serial for DFPlayer-mini mp3 module 
	mp3_set_volume (25);

	// Setting sensor outputs
	pinMode(S0, OUTPUT);
	pinMode(S1, OUTPUT);
	pinMode(S2, OUTPUT);
	pinMode(S3, OUTPUT);
  pinMode(OE, OUTPUT);
	// Setting the OUT as an input
	pinMode(OUT, INPUT);
 	
 	// Setting frequency scaling to 20% (for atmega controllers)
	digitalWrite(S0, HIGH);
	digitalWrite(S1, LOW);
  digitalWrite(OE, HIGH);
 
 	//pinmode for actuators
	pinMode(tail, OUTPUT);
	pinMode(mouth, OUTPUT);
	pinMode(EML_Table, OUTPUT);
  digitalWrite(EML_Table, HIGH);

	Serial.println("Started");
  //HC12.println("HC_12_initiated");
}

void loop()
{  
  HC_12_loop();
	color_sensor();
  if(what_color == 'B' && is_fish_done == false)
  {
    is_fish_done = true;
    Serial.println("fish is singing");
    HC12.println(cnfrm);
    mp3_play(4);
    digitalWrite(EML_Table, LOW);
    fish_animatronic();
    mp3_stop();
    time = millis(); 

    what_color = 'N';
    is_fish_done = false;
  }

  if(what_color == 'W')
  {
    for(int fish = 0; fish < 3; fish++)
    {
      Serial.println("color isn`t correct");
      digitalWrite(tail, HIGH);
      delay(random(500));
      digitalWrite(tail, LOW);
      delay(random(500));
    }
    delay(2000);
  }

  if (millis() - time > 10000) digitalWrite(EML_Table, HIGH); //time for EML still open

  switch (what_color) 
  {
    case 'W':
      Serial.println("WHITE");
      break;
    case 'R':
      Serial.println("RED");
      break;
    case 'G':
      Serial.println("GREEN");
      break;
    case 'B':
      Serial.println("BLUE");
      break;    
    case 'N':
      Serial.println("NOTHING");
      break;
  }
}

void color_sensor() 
{
  // Setting RED (R) filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  // Reading the output frequency
  redFrequency = pulseIn(OUT, LOW);
  //Serial.print("R="); Serial.print(redFrequency);

  
  // Setting GREEN (G) filtered photodiodes to be read
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  greenFrequency = pulseIn(OUT, LOW);
  //Serial.print(" G="); Serial.print(greenFrequency);
 
  // Setting BLUE (B) filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  blueFrequency = pulseIn(OUT, LOW);
  //Serial.print(" B="); Serial.print(blueFrequency);

  // Setting WHITE (W) no filtered photodiodes to be read
  digitalWrite(S2,HIGH);
  digitalWrite(S3,LOW);
  // Reading the output frequency
  whiteFrequency = pulseIn(OUT, LOW);
  //Serial.print(" W="); Serial.println(whiteFrequency);

  if((redFrequency <= 300 && greenFrequency <= 300 && blueFrequency <= 300 && whiteFrequency <= 300 && redFrequency > 0 && greenFrequency > 0 && blueFrequency > 0 && whiteFrequency > 0))
  {
    what_color = 'W';
  }
  else if((redFrequency < 300 || blueFrequency < 300) && greenFrequency > 300 && whiteFrequency > 300 && redFrequency > 0 && greenFrequency > 0 && blueFrequency > 0 && whiteFrequency > 0)
  {
    what_color = 'B'; // means R or B
  }
  else if(redFrequency > 300 && greenFrequency < 300 && blueFrequency > 300 && whiteFrequency > 300 && redFrequency > 0 && greenFrequency > 0 && blueFrequency > 0 && whiteFrequency > 0)
  {
    what_color = 'G';
  }
  else what_color = 'N';
}

void fish_animatronic()
{
  for(int fish = 0; fish < 9; fish++)
  {
    digitalWrite(tail, HIGH);
    delay(random(50,300));
    digitalWrite(mouth, HIGH);
    delay(random(50, 300));
    digitalWrite(mouth, LOW);
    delay(random(50, 300));
    digitalWrite(mouth, HIGH);
    delay(random(50, 300));
    digitalWrite(mouth, LOW);
    delay(random(50, 300));
    digitalWrite(tail, LOW);
    delay(random(50, 300));
    digitalWrite(mouth, HIGH);
    delay(random(50, 300));
    digitalWrite(mouth, LOW);
    delay(random(50, 300));
    digitalWrite(mouth, HIGH);
    delay(random(50, 300));
    digitalWrite(mouth, LOW);
    delay(random(50, 300));
  }
}

void HC_12_loop() 
{              //recieve something from hc-12 inerface
  while (HC12.available())
  {
    char inChar = HC12.read(); //store each bite in var
    temp_string += inChar;     //add to string
    Serial.print(inChar); //Send each recieved byte back
    if (inChar == '#')       //if stop byte recieved
    {
      Serial.print(temp_string);
      Serial.println(" - copy that");
      if (temp_string == MCS_karp_play && is_fish_done != true)  //compare string with a known commands
      {
        Serial.println("Admin has been activated FISH");
        is_fish_done = true;
        Serial.println("fish is singing");
        mp3_play(4);
        digitalWrite(EML_Table, LOW);
        fish_animatronic();
        mp3_stop();
        time = millis(); 

        what_color = 'N';
        is_fish_done = false;
      }
    temp_string = "";     //then clear the string
    }
  }
}

void HC_12_test()
{
  if(HC12.available())
  {
    char inChar = HC12.read();
    Serial.print(inChar); //Send each recieved byte back
  }
}