#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <SoftwareSerial.h>

int pinCS = 9;
int numberOfHorizontalDisplays = 4; // теперь у нас по-горизонтали 4 матрицы
int numberOfVerticalDisplays = 1; // а по-вертикали, по-прежнему, одна
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

SoftwareSerial Serial_HC(6, 7); // (Rx_pin, Tx_pin) //using a softwareSerial instead of serial, because of debugging through console and uploading a sketch

////////////////Scroll setup/////////////////
String tape = "Ready"; // текст, который будет плыть
String tape2 = "Time is over"; // текст, который будет плыть

int wait = 50; // время между крайними перемещениями букв
int spacer = 1; // расстояние между буквами
int width = 5 + spacer; // размер шрифта

/////////////////////timer setup////////////////////
int timeInSeconds = 3600; //Put your time for timer in seconds. i.e. 60 minutes = 3600 seconds
int secondsLeft = timeInSeconds;
//Time variables
unsigned long millisNow = 0;
unsigned long millisPrew = 0;
int interval = 1000;

//////////////////HC-21 setup////////////////
String temp_string = ""; //variable to store information recieved form serial and compare it

//Slave-Master strings
String time_ends = "time_ends#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte

//Master-Slave strings
String tmr_strt = "tmr_strt#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String tmr_pls_5m = "tmr_pls_5m#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String tmr_rst = "tmr_rst#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String tmr_stp = "tmr_stp#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte

bool timer_start_flag = false;
bool timer_reset_flag = false;
bool timer_stop_flag = false;

void setup() {
    Serial.begin(9600);
    Serial_HC.begin(9600);

    matrix.setIntensity(1); // яркость
    matrix.setRotation( 0, 1 );
    matrix.setRotation( 1, 1 );
    matrix.setRotation( 2, 1 );
    matrix.setRotation( 3, 1 );
}

void loop()
{
    HC_12_loop();
    if(!timer_start_flag && !timer_stop_flag)
    {
        ready();
        timer_reset_flag = false;
        timer_stop_flag = false;
    }

    if(timer_start_flag && !timer_stop_flag)
    {
        timer();
        timer_stop_flag = false;
    }

    if(timer_reset_flag)
    {
        secondsLeft = timeInSeconds;
        timer_start_flag = false;
        timer_reset_flag = false;
        timer_stop_flag = false;
    }

    if(timer_stop_flag)
    {
        //do nothing and wait
    }

}

void timer()
{
    byte minutes = 0;
    byte seconds = 0;
    
    if(secondsLeft>=0)
    {
        millisNow = millis();
        if(millisNow - millisPrew > interval) 
        {
            millisPrew = millisNow;
            minutes = secondsLeft/60;
            seconds = secondsLeft%60;

            Serial.print(minutes/10);
            Serial.print(minutes%10);
            Serial.print(seconds/10);
            Serial.println(seconds%10);

            matrix.fillScreen(LOW);
            matrix.drawChar(1, 0, minutes/10+48, HIGH, LOW, 1); //ascii "0" char number is 48, 1=49, 2=50, etc.
            matrix.drawChar(8, 0, minutes%10+48, HIGH, LOW, 1);

            matrix.drawPixel(15, 1, HIGH);
            matrix.drawPixel(15, 2, HIGH);
            matrix.drawPixel(15, 4, HIGH);
            matrix.drawPixel(15, 5, HIGH);

            matrix.drawPixel(16, 1, HIGH);
            matrix.drawPixel(16, 2, HIGH);
            matrix.drawPixel(16, 4, HIGH);
            matrix.drawPixel(16, 5, HIGH);

            //matrix.drawChar(9, 0, 58, HIGH, LOW, 1); // ascii ":" char
            matrix.drawChar(19, 0, seconds/10+48, HIGH, LOW, 1);
            matrix.drawChar(26, 0, seconds%10+48, HIGH, LOW, 1);
            matrix.write();

            secondsLeft--;
        }
    }

    if(secondsLeft <= 0)
    {
        tape = "Time is over";
        timeisover();
    }
}

void ready()
{
    matrix.drawChar(1, 0, tape[0], HIGH, LOW, 1);
    matrix.drawChar(7, 0, tape[1], HIGH, LOW, 1);
    matrix.drawChar(13, 0, tape[2], HIGH, LOW, 1);
    matrix.drawChar(19, 0, tape[3], HIGH, LOW, 1);
    matrix.drawChar(25, 0, tape[4], HIGH, LOW, 1);
    matrix.write();
}

void timeisover()
{
    byte minutes = 0;
    byte seconds = 0;

    millisPrew = millisNow;
    minutes = secondsLeft/60;
    seconds = secondsLeft%60;

    Serial.print(minutes/10);
    Serial.print(minutes%10);
    Serial.print(seconds/10);
    Serial.println(seconds%10);

    matrix.fillScreen(LOW);
    matrix.drawChar(1, 0, minutes/10+48, HIGH, LOW, 1); //ascii "0" char number is 48, 1=49, 2=50, etc.
    matrix.drawChar(8, 0, minutes%10+48, HIGH, LOW, 1);

    matrix.drawPixel(15, 1, HIGH);
    matrix.drawPixel(15, 2, HIGH);
    matrix.drawPixel(15, 4, HIGH);
    matrix.drawPixel(15, 5, HIGH);

    matrix.drawPixel(16, 1, HIGH);
    matrix.drawPixel(16, 2, HIGH);
    matrix.drawPixel(16, 4, HIGH);
    matrix.drawPixel(16, 5, HIGH);

    //matrix.drawChar(9, 0, 58, HIGH, LOW, 1); // ascii ":" char
    matrix.drawChar(19, 0, seconds/10+48, HIGH, LOW, 1);
    matrix.drawChar(26, 0, seconds%10+48, HIGH, LOW, 1);
    matrix.write();
}

void scroll() {
    for ( int i = 0 ; i < width * tape2.length() + matrix.width() - 1 - spacer; i++ ) {
        matrix.fillScreen(LOW);

        int letter = i / width;
        int x = (matrix.width() - 1) - i % width;
        int y = (matrix.height() - 8) / 2; // center the text vertically

        while ( x + width - spacer >= 0 && letter >= 0 ) {
            if ( letter < tape2.length() ) {
                matrix.drawChar(x, y, tape2[letter], HIGH, LOW, 1);
            }
            letter--;
            x -= width;
        }

        matrix.write();
        delay(wait);
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

      if (temp_string == tmr_strt)  //compare string with a known commands
      {
        timer_start_flag = true;
        timer_stop_flag = false;
      }

      if (temp_string == tmr_pls_5m)  //compare string with a known commands
      {
        secondsLeft += 300;
      }

      if (temp_string == tmr_rst)  //compare string with a known commands
      {
        timer_reset_flag = true;
      }

      if (temp_string == tmr_stp)  //compare string with a known commands
      {
        timer_stop_flag = true;
      }
    temp_string = "";     //then clear the string
    }
  }
}