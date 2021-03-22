#include <DFPlayer_Mini_Mp3.h>
#include <SoftwareSerial.h>

SoftwareSerial HC12(2, 3); // (Rx_pin, Tx_pin) //using a softwareSerial instead of serial, because of debugging through console and uploading a sketch
SoftwareSerial esp_wifi(44, 46); // (Rx_pin, Tx_pin) //using a softwareSerial instead of serial, because of debugging through console and uploading a sketch

//=======================MCS_IN-OUTS==============================
//========================MCS-left_OUTs
const int MOSF1_puchlya_door_D53 = 53; //MOSFET 1
const int MOSF2_exit_door_D51 = 51; //MOSFET 2
const int MCS_D49 = 49; //MOSFET 3
const int MCS_D47 = 47; //MOSFET 4
const int MCS_D45 = 45; //MOSFET 5
const int MCS_D43 = 43; //MOSFET 6
const int MCS_D41 = 41; //MOSFET 7
const int MCS_D39 = 39; //MOSFET 8
const int MCS_D37 = 37; //
const int MCS_D35 = 35; //
const int MCS_D33 = 33; //
const int MCS_D31 = 31; //

//=======================MCS-down_MOSFETS
const int MCS_D13 = 13;
const int MCS_D12 = 12;
const int MCS_D11 = 11;
const int MCS_D10 = 10;
const int MCS_D9 = 9;
const int MCS_D8 = 8;
const int MCS_D7 = 7;
const int MCS_D6 = 6;
const int MCS_D5 = 5;
const int MCS_D4 = 4;
//const int MCS_D3 = 3;
//const int MCS_D2 = 2;
const int MCS_D32 = 32;
const int MCS_D34 = 34;
const int MCS_D36 = 36;
const int MCS_D38 = 38;

//=======================MCS-left-down_dig_outs
const int MCS_D22 = 22;
const int MCS_D24 = 24;
const int MCS_D25 = 26;
const int MCS_D28 = 28;
const int MCS_D30 = 30;

//=======================MCS-up_an_INs
#define MCS_pict1_A0 A0
#define MCS_pict2_A1 A1
#define MCS_pict3_A2 A2
#define MCS_pict4_A3 A3
#define MCS_A4 A4
#define MCS_A5 A5
#define MCS_A6 A6
#define MCS_A7 A7
#define MCS_A8 A8
#define MCS_A9 A9
#define MCS_A10 A10
#define MCS_A11 A11
#define MCS_A12 A12
#define MCS_A13 A13
#define MCS_A14 A14
#define MCS_A15 A15

//=======================MCS-up_an_INs

const int MCS_D44 = 44;
const int MCS_D46 = 46;
const int MCS_D48 = 48;
const int MCS_D50 = 50;
const int MCS_D52 = 52;

String string;
String string_reply;

//GF_Game_console
String play_vid_1 = "play_vid_1#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String play_vid_2 = "play_vid_2#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String play_vid_3 = "play_vid_3#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String pleer_on = "pleer_on#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String pleer_off = "pleer_off#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte

//GF_karp_nolib
String MCS_karp_play = "MCS_karp_play#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String cnfrm = "#SL_karp_done#"; //send command string should be "#xx...x#" format - for sure to correctly recieve a command. 1st "#" byte clears all junk before comparing, may work without it

//GF_lab_panel
//Slave-Master strings
String but21_done = "but21_done#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String open_port = "open_port#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String close_port = "close_port#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String but9_done = "but9_done#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
//Master-Slave strings
String but21_open = "but21_op#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String but9_open = "but9_op#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String reset_lab_panel = "reset_lab_panel#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte

//GF_magic_circle
//Slave-Master strings
String mgc_crcl_done = "mgc_crcl_done#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte

//GF_snack_automat
String temp_string = ""; //variable to store information recieved form serial and compare it
String activate_snack = "act_sn#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String open_snack = "open_sn#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String reset_snack = "res_sn#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String snack_done = "sn_done#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte

//GF_Telephone_display
String MCS_TV_play = "MCS_TV_play#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String cnfrm_TV = "#SL_TV_done#"; //send command string should be "#xx...x#" format - for sure to correctly recieve a command. 1st "#" byte clears all junk before comparing, may work without it

//GF_Teleport strings
String Tele_mov1 = "Tele_mov1#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String Tele_mov2 = "Tele_mov2#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String Tele_mov3 = "Tele_mov3#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String Tele_mov4 = "Tele_mov4#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String Tele_mov5 = "Tele_mov5#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String Tele_mov6 = "Tele_mov6#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String Tele_on = "Tele_on#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte

//GF_timer_max7219
//Slave-Master strings
String time_is_over = "tm_ovr#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
//Master-Slave strings
String tmr_strt = "tmr_strt#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String tmr_pls_5m = "tmr_pls_5m#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String tmr_rst = "tmr_rst#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String tmr_stp = "tmr_stp#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte

//GF_remote_XY
const int num_cmnds = 4;
String commands_array[num_cmnds] = {"tmr_strt#", "tmr_pls_5m#", "tmr_rst#", "tmr_stp#"};

const int num_cmnds2 = 14;
String remotexy[num_cmnds2] = 
{ "tmr_strt#", 
  "tmr_stp#", 
  "tmr_rst#", 
  "tmr_pls_5m#",
  "console_btn#",
  "pictures_btn#",
  "pukhlya_btn#",
  "Kapr_btn#",
  "telephone_btn#",
  "snack_btn#",
  "code_panel_btn#",
  "twone_btn#",
  "nine_btn#",
  "magic_circle_btn#",
};

void setup() {
    Serial.begin(9600);						  //UART
    Serial1.begin(9600); //pl1
    Serial2.begin(9600); //pl2
    Serial3.begin(9600); //ESP8266?
    HC12.begin(9600); //HC12

    mp3_set_serial(Serial1);
    delay(10);
    mp3_set_volume (20);
    mp3_set_serial(Serial2);
    delay(10);
    mp3_set_volume (20);

    pinMode(MCS_pict1_A0, INPUT_PULLUP);
    pinMode(MCS_pict2_A1, INPUT_PULLUP);
    pinMode(MCS_pict3_A2, INPUT_PULLUP);
    pinMode(MCS_pict4_A3, INPUT_PULLUP);

  Serial.println("OSU_loaded");

  //Serial.println("\n[memCheck]");
  //Serial.println(freeRam()); // вызываем ф-ю ниже
}

int freeRam () { //функция, показываюзая количество свободной ОЗУ

  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
//void(* resetFunc) (void) = 0;//объявляем функцию reset с адресом 0 - софтовая перезагрузка

void loop() 
{
  HC12_loop();
  test_pic();
}

void HC12_loop()
{              //recieve something from hc-12 inerface
  while (Serial3.available())
  {
    char inChar = Serial3.read(); //store each bite in var
    temp_string += inChar;     //add to string
    //Serial.print(inChar); //Send each recieved byte back
    
  if (inChar == '#')       //if stop byte recieved
    {
      Serial.print(temp_string);
      Serial.println(" - copy that");

      if (temp_string == tmr_strt)  //compare string with a known commands
      {
        Serial3.println("message");
      }

      temp_string = "";     //then clear the string
    }
  }
}

void test_pic()
{
  int p1 = digitalRead(MCS_pict1_A0);
  int p2 = digitalRead(MCS_pict2_A1);
  int p3 = digitalRead(MCS_pict3_A2);
  int p4 = digitalRead(MCS_pict4_A3);
  Serial.print(p1);
  Serial.print(" ");
  Serial.print(p2);
  Serial.print(" ");
  Serial.print(p3);
  Serial.print(" ");
  Serial.println(p4);
}