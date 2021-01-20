/*
   -- GF_remote_XY --
   
   This source code of graphical user interface 
   has been generated automatically by RemoteXY editor.
   To compile this code using RemoteXY library 2.4.3 or later version 
   download by link http://remotexy.com/en/library/
   To connect using RemoteXY mobile app by link http://remotexy.com/en/download/                   
     - for ANDROID 4.5.1 or later version;
     - for iOS 1.4.1 or later version;
    
   This source code is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.    
*/

//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// определение режима соединения и подключение библиотеки RemoteXY 
#define REMOTEXY_MODE__ESP8266WIFI_LIB
#include <ESP8266WiFi.h>
#include <RemoteXY.h>
#include <SoftwareSerial.h>

SoftwareSerial Serial_HC(D1, D2); // (Rx_pin, Tx_pin) //using a softwareSerial instead of serial, because of debugging through console and uploading a sketch

// настройки соединения 
#define REMOTEXY_WIFI_SSID "ac"
#define REMOTEXY_WIFI_PASSWORD "docentt2"
#define REMOTEXY_SERVER_PORT 6377


// конфигурация интерфейса  
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =
  { 255,14,0,0,0,139,1,10,13,5,
  1,2,5,14,52,16,1,119,31,83,
  116,97,114,116,0,131,1,2,2,28,
  7,1,2,31,208,151,208,176,208,191,
  209,131,209,129,208,186,0,1,2,6,
  58,31,12,1,49,31,82,101,115,101,
  116,0,1,2,6,76,31,13,1,189,
  31,43,53,109,105,110,0,1,2,5,
  36,52,16,1,10,31,83,116,111,112,
  0,131,0,31,2,31,7,2,2,31,
  208,154,208,181,209,128,209,131,208,178,
  208,176,208,189,208,189,209,143,0,129,
  0,45,11,13,4,2,17,208,161,209,
  130,208,176,209,130,209,131,209,129,0,
  1,2,3,15,35,6,2,2,31,49,
  46,208,154,208,190,208,189,209,129,208,
  190,208,187,209,140,0,1,2,3,23,
  35,6,2,2,31,50,46,208,154,208,
  176,209,128,209,130,208,184,208,189,208,
  184,0,1,2,3,31,35,6,2,2,
  31,51,46,208,159,209,131,209,133,208,
  187,209,143,0,1,2,3,39,35,6,
  2,2,31,52,46,208,154,208,176,209,
  128,208,191,0,1,2,3,47,35,6,
  2,2,31,53,46,208,162,208,181,208,
  187,208,181,209,132,208,190,208,189,0,
  1,2,3,55,35,6,2,2,31,54,
  46,32,208,161,208,189,208,181,208,186,
  32,208,176,208,178,209,130,208,190,208,
  188,0,1,2,3,63,35,6,2,2,
  31,55,46,32,208,154,208,190,208,180,
  32,208,191,208,176,208,189,208,181,208,
  187,209,140,0,1,2,3,71,35,6,
  2,2,31,56,46,32,50,49,32,209,
  129,208,184,208,188,208,178,208,190,208,
  187,0,1,2,3,79,35,6,2,2,
  31,57,46,32,208,156,208,181,208,188,
  208,190,0,1,2,3,87,35,6,2,
  2,31,49,48,46,32,208,156,208,176,
  208,179,32,208,186,209,128,209,131,208,
  179,0 };
  
// структура определяет все переменные и события вашего интерфейса управления 
struct btn_names{
    // input variables
  uint8_t start_btn; // =1 если кнопка нажата, иначе =0 
  uint8_t reset_btn; // =1 если кнопка нажата, иначе =0 
  uint8_t pls_5m_btn; // =1 если кнопка нажата, иначе =0 
  uint8_t stp_btn; // =1 если кнопка нажата, иначе =0 
  uint8_t console_btn; // =1 если кнопка нажата, иначе =0 
  uint8_t pictures_btn; // =1 если кнопка нажата, иначе =0 
  uint8_t pukhlya_btn; // =1 если кнопка нажата, иначе =0 
  uint8_t Kapr_btn; // =1 если кнопка нажата, иначе =0 
  uint8_t telephone_btn; // =1 если кнопка нажата, иначе =0 
  uint8_t snack_btn; // =1 если кнопка нажата, иначе =0 
  uint8_t code_panel_btn; // =1 если кнопка нажата, иначе =0 
  uint8_t twone_btn; // =1 если кнопка нажата, иначе =0 
  uint8_t nine_btn; // =1 если кнопка нажата, иначе =0 
  uint8_t magic_circle_btn; // =1 если кнопка нажата, иначе =0 
    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;
#pragma pack(pop)
/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////

const int num_cmnds = 4;
String commands_array[num_cmnds] = {"tmr_strt#", "tmr_pls_5m#", "tmr_rst#", "tmr_stp#"};

byte btn_divider = 0;

String tmr_strt = "tmr_strt#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String tmr_pls_5m = "tmr_pls_5m#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String tmr_rst = "tmr_rst#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String tmr_stp = "tmr_stp#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte

void setup()
{
  Serial.begin(9600);
  Serial_HC.begin(9600);
  RemoteXY_Init ();
}

void loop2() 
{ 
  Serial.println("1");
  delay(500);
}

void loop() 
{ 
  RemoteXY_Handler ();

  if(RemoteXY.start_btn==1)
  {
    delay(300); //for pressing button once at a time
    btn_divider++;

    if(btn_divider == 2) //command comes twice by a protocol
    {
      Serial.println(commands_array[0]);
      Serial_HC.print(commands_array[0]);
      btn_divider = 0;
    }
  }

  if(RemoteXY.reset_btn==1)
  {
    delay(300); //for pressing button once at a time
    btn_divider++;

    if(btn_divider == 2) //command comes twice by a protocol
    {
      Serial.println("reset_btn");
      Serial_HC.print("tmr_rst#");
      btn_divider = 0;
    }
  }

  if(RemoteXY.stp_btn==1)
  {
    delay(300); //for pressing button once at a time
    btn_divider++;

    if(btn_divider == 2) //command comes twice by a protocol
    {
      Serial.println("stp_btn");
      Serial_HC.print("tmr_stp#");
      btn_divider = 0;
    }
  }

  if(RemoteXY.pls_5m_btn==1)
  {
    delay(300); //for pressing button once at a time
    btn_divider++;

    if(btn_divider == 2) //command comes twice by a protocol
    {
      Serial.println("pls_5m_btn");
      Serial_HC.print("tmr_pls_5m#");
      btn_divider = 0;
    }
  }


}
