#include <SoftwareSerial.h>
#include <Keypad.h>

////////////HC12/////////////
SoftwareSerial HC12(2, 4); // (Rx_pin, Tx_pin) //using a softwareSerial instead of serial, because of debugging through console and uploading a sketch

String temp_string = ""; //variable to store information recieved form serial and compare it
char inChar;

//Command strings
String red1 = "1#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String red2 = "#2#"; //send command string should be "#xx...x#" format - for sure to correctly recieve a command. 1st "#" byte clears all junk before comparing, may work without it
String cnfrm = "$";

const int irled = 3; //ir led transmitting commands to TV

/////////////keypad/////////
int flag = 0;
int flag_true = 0;
int flag_false = 0;

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
  {'*','0','#'}
};
byte rowPins[ROWS] = {10, 9, 11, 12}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 6, 8}; //connect to the column pinouts of the keypad

Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup() 
{
  Serial.begin(9600); //initiating serial
  HC12.begin(9600); //initiating software serial
}

void loop()
{
	keypad_password();
}

void HC_12_do_smth() 
{              //recieve something from hc-12 inerface
  while (HC12.available())
  {
    char inChar = HC12.read(); //store each bite in var
    temp_string += inChar;     //add to string
    if (inChar == '#')		   //if stop byte recieved
    {
      if (temp_string == red1)  //compare string with a known commands
      {							// and do something

    	 HC12.print(red2);		//send a command back
      }
 	temp_string = "";			//than clear the string
    }
  }
}


void keypad_password(){
  char pressed=customKeypad.getKey();
  
 if(flag_true==0){if(pressed==char1){flag_true=1;}}  //1st symbol of password 
 if(flag_true==1){if(pressed==char2){flag_true=2;}}  //2nd symbol of password
 if(flag_true==2){if(pressed==char3){flag_true=3;}}  //3rd symbol of password
 if(flag_true==3){if(pressed==char4){flag_true=4;}}  //4
 if(flag_true==4){if(pressed==char5){flag_true=5;}}  //5
 if(flag_true==5){if(pressed==char6){flag_true=6;}}  //6
 if(flag_true==6){if(pressed==char7){flag_true=7;}}  //7
 if(flag_true==7){if(pressed==char8){flag_true=8;}}  //8
 if(flag_true==8){if(pressed==char9){flag_true=9;}}  //9
 if(flag_true==9){if(pressed==char10){flag_true=10;}}  //10

  if(pressed!='\0')  {
    flag++;    
    Serial.println(pressed);
  }

 if(pressed==char_confirm){
  if(flag_true==10){
    Serial.println("CORRECT PASSWORD");
    flag = 0;
    flag_true = 0;
    flag_false = 0;
  }
  else if(flag_true!=10){
    Serial.println("WRONG PASSWORD");
    flag = 0;
    flag_true = 0;
    flag_false = 0;
  }
 }
}