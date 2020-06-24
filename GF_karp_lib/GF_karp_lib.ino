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
https://voltiq.ru/connecting-tcs230-to-arduino/
///////////////////////////////////*/

#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h> //lib for dfplayer
#include "MD_TCS230.h" //libs for color sensor
#include "FreqCount.h" ////libs for color sensor

SoftwareSerial HC12(10, 9); // (Rx_pin, Tx_pin) //using a softwareSerial instead of serial, because of debugging through console and uploading a sketch
SoftwareSerial DF_player(2, 4); // (Rx_pin, Tx_pin) //using a softwareSerial instead of serial, because of debugging through console and uploading a sketch

String temp_string = ""; //variable to store information recieved form serial and compare it
char inChar;

//Command strings
String MCS_TV_play = "MCS_karp_play#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String cnfrm = "#SL_karp_done#"; //send command string should be "#xx...x#" format - for sure to correctly recieve a command. 1st "#" byte clears all junk before comparing, may work without it

//////////COLOR SENSOR/////////////

// Color definitions
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF
#define GREY     0xC618
 
// Pin definitions
#define S0_OUT 11
#define S1_OUT 7
#define S2_OUT 6
#define S3_OUT 12
#define OE_OUT 8  // LOW = ENABLED 
#define OUT 13

// Stores frequency read by the photodiodes
int R = 0;
int G = 0;
int B = 0;

//actuator definitions
#define motor1 3 //fish tail
#define motor2 5 //fish mouth
#define EML_Table A0 //Table - magnetic lock
 
MD_TCS230 CS(S2_OUT, S3_OUT, OE_OUT);
int state = 0;


void setup() 
{
	Serial.begin(9600); //initiating serial
	HC12.begin(9600); //initiating software serial
	DF_player.begin(9600); //initiating software serial

	mp3_set_serial (DF_player);  //set Serial for DFPlayer-mini mp3 module 
	mp3_set_volume (15);

	CS.begin(); //initiating color sensor
	CS.read(); //start to read

	pinMode(motor1, OUTPUT);
	pinMode(motor2, OUTPUT);
	pinMode(EML_Table, OUTPUT);

	Serial.println("Started");
	Serial.println("Ready to play");
}

void loop() 
{

	if(state ==0)
	{
	  if (CS.available()) 
	  {
	   sensorData  sd;
	   CS.getRaw(&sd); 
	   CS.setDarkCal(&sd); 
	   Serial.println("Black Calibration Set");
	   state=1;
	   delay(5000);
	   CS.read();
	  }
	}
	else if(state == 1)
	{
	 if (CS.available()) 
	 {
	   sensorData  sd;
	   CS.getRaw(&sd); 
	   CS.setWhiteCal(&sd); 
	   Serial.println("White Calibration Set");
	   state=2;
	  }
	}
	else
	{
	  readSensor();
	}
}

void readSensor()
{
  static  bool  waiting = false;
  
  if (!waiting)
  {
    CS.read();
    waiting = true;
  }
  else
  {
    if (CS.available()) 
    {
      colorData  rgb;
      
      CS.getRGB(&rgb);
      Serial.print("RGB [");
      Serial.print(rgb.value[TCS230_RGB_R]);
      Serial.print(",");
      Serial.print(rgb.value[TCS230_RGB_G]);
      Serial.print(",");
      Serial.print(rgb.value[TCS230_RGB_B]);
      Serial.println("]");
      
      R = rgb.value[TCS230_RGB_R];
      G = rgb.value[TCS230_RGB_G];
      B = rgb.value[TCS230_RGB_B];

      waiting = false;
    }
  }
}