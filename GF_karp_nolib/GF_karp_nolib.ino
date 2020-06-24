/*///////////////////////////////////
Description:
Project for quest room - funny talking fish with color sensor and 433mhz HC-12 communication .

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

SoftwareSerial HC12(10, 9); // (Rx_pin, Tx_pin) //using a softwareSerial instead of serial, because of debugging through console and uploading a sketch
SoftwareSerial DF_player(2, 4); // (Rx_pin, Tx_pin) //using a softwareSerial instead of serial, because of debugging through console and uploading a sketch

String temp_string = ""; //variable to store information recieved form serial and compare it
char inChar;

//Command strings
String MCS_TV_play = "MCS_karp_play#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
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

//actuator definitions
#define motor1 3 //fish tail
#define motor2 5 //fish mouth
#define EML_Table A0 //Table - magnetic lock

void setup() 
{
	Serial.begin(9600); //initiating serial
	HC12.begin(9600); //initiating software serial
	DF_player.begin(9600); //initiating software serial

	mp3_set_serial (DF_player);  //set Serial for DFPlayer-mini mp3 module 
	mp3_set_volume (15);

	// Setting sensor outputs
	pinMode(S0, OUTPUT);
	pinMode(S1, OUTPUT);
	pinMode(S2, OUTPUT);
	pinMode(S3, OUTPUT);
	// Setting the OUT as an input
	pinMode(OUT, INPUT);
 	
 	// Setting frequency scaling to 20% (for atmega controllers)
	digitalWrite(S0, HIGH);
	digitalWrite(S1, LOW);
 
 	//pinmode for actuators
	pinMode(motor1, OUTPUT);
	pinMode(motor2, OUTPUT);
	pinMode(EML_Table, OUTPUT);

	Serial.println("Started");
}

void loop()
{
	color_sensor();
}

void color_sensor() 
{
  // Setting RED (R) filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  // Reading the output frequency
  redFrequency = pulseIn(OUT, LOW);
  Serial.print("R = ");
  Serial.print(redFrequency);
  delay(100);
  
  // Setting GREEN (G) filtered photodiodes to be read
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  greenFrequency = pulseIn(OUT, LOW);
  Serial.print(" G = ");
  Serial.print(greenFrequency);
  delay(100);
 
  // Setting BLUE (B) filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  blueFrequency = pulseIn(OUT, LOW);
  Serial.print(" B = ");
  Serial.println(blueFrequency);
  delay(100);

  delay(1000);
}
