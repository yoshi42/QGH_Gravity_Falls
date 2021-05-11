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
#include <Wire.h>
#include <SparkFun_APDS9960.h>

SoftwareSerial HC12(9, 10); // (Rx_pin, Tx_pin) //using a softwareSerial instead of serial, because of debugging through console and uploading a sketch
SoftwareSerial DF_player(4, 2); // (Rx_pin, Tx_pin) //using a softwareSerial instead of serial, because of debugging through console and uploading a sketch

String temp_string = ""; //variable to store information recieved form serial and compare it

//Command strings
String MCS_karp_play = "MCS_karp_play#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String cnfrm = "#SL_karp_done#"; //send command string should be "#xx...x#" format - for sure to correctly recieve a command. 1st "#" byte clears all junk before comparing, may work without it


//actuator definitions
#define tail 5 //fish tail
#define mouth 3 //fish mouth
#define EML_Table A0 //Table - magnetic lock

bool is_fish_done = false;
unsigned  long time = 0;

// Color Sensor
SparkFun_APDS9960 apds = SparkFun_APDS9960();
uint16_t ambient_light = 0;
uint16_t red_light = 0;
uint16_t green_light = 0;
uint16_t blue_light = 0;

char what_color = '0';

void setup() {
  
  // Initialize Serial port
  Serial.begin(9600);

  DF_player.begin(9600); //initiating software serial
  HC12.begin(9600); //initiating software serial

  mp3_set_serial (DF_player);  //set Serial for DFPlayer-mini mp3 module 
  mp3_set_volume (25);

  //pinmode for actuators
  pinMode(tail, OUTPUT);
  pinMode(mouth, OUTPUT);
  pinMode(EML_Table, OUTPUT);
  digitalWrite(EML_Table, HIGH);

  //Color Sensor
  Serial.println();
  Serial.println(F("--------------------------------"));
  Serial.println(F("SparkFun APDS-9960 - ColorSensor"));
  Serial.println(F("--------------------------------"));
  
  // Initialize APDS-9960 (configure I2C and initial values)
  if ( apds.init() ) {
    Serial.println(F("APDS-9960 initialization complete"));
  } else {
    Serial.println(F("Something went wrong during APDS-9960 init!"));
  }
  
  // Start running the APDS-9960 light sensor (no interrupts)
  if ( apds.enableLightSensor(false) ) {
    Serial.println(F("Light sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during light sensor init!"));
  }
  
  // Wait for initialization and calibration to finish
  delay(500);
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

  if(what_color == 'W' || what_color == 'R' || what_color == 'G')
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
}

void color_sensor() {
  
  // Read the light levels (ambient, red, green, blue)
  if (  !apds.readAmbientLight(ambient_light) ||
        !apds.readRedLight(red_light) ||
        !apds.readGreenLight(green_light) ||
        !apds.readBlueLight(blue_light) ) {
    Serial.println("Error reading light values");
  } 
  else {
    Serial.print("Ambient: ");
    Serial.print(ambient_light);
    Serial.print(" Red: ");
    Serial.print(red_light);
    Serial.print(" Green: ");
    Serial.print(green_light);
    Serial.print(" Blue: ");
    Serial.println(blue_light);

    Serial.print("Ambient: ");
    Serial.print(ambient_light);
    Serial.print(" Red: ");
    Serial.print(ambient_light/red_light);
    Serial.print(" Green: ");
    Serial.print(ambient_light/green_light);
    Serial.print(" Blue: ");
    Serial.println(ambient_light/blue_light);
  }
  
  // Wait 1 second before next reading
  delay(500);

  if(ambient_light >= 200 && ambient_light/red_light < 2 && ambient_light/green_light < 2 && ambient_light/blue_light < 2) {what_color = 'W';}
  else if(ambient_light >= 200 && ambient_light/red_light < 2 && ambient_light/green_light >= 2 && ambient_light/blue_light >= 2) {what_color = 'R';}
  else if(ambient_light >= 200 && ambient_light/red_light >= 2 && ambient_light/green_light < 2 && ambient_light/blue_light >= 2) {what_color = 'G';}
  else if(ambient_light >= 200 && ambient_light/red_light >= 2 && ambient_light/green_light >= 2 && ambient_light/blue_light < 2) {what_color = 'B';}
  else if(ambient_light >= 200) {what_color = 'W';}
  else what_color = 'N';

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
      Serial.println("NOTHINGGGG");
      break;
  }
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