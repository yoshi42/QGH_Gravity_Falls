/* 
Система управління квест-кімнатою, яка відповідає за зв'язок між всіма девайсами та загальну логіку роботи квесту.

Алгоритм:
  1. Адмін натискає старт - Стартує квест: таймер, запускається музика 0001_фонова_старт.mp3
  2. Пройти ігровий автомат, подивитися відео.
  3. Виставити картини - відкриваєтсья ящик в потолку (підказка про пухлю)
  4. Посвітити в карпа - відкривається стол, отримуємо пухлю
  5. Прикласти пухлю - відкриваються двері (змінюється фонова музика 0002_кімната_Діпера.mp3 -якщо ми до цього не відкрили уже лабораторію), отримуємо щоденник
  6. Подзвонити по телефону - вмикається відео з камер. Отримуємо код від снекового автомата
  7. Ввести пароль в снек - запускається movie1.mp4 "Ефект порталу", відкриваються двері в лабораторію (змінюється фонова музика 0003_лабораторія.mp3),
  8. Ввести пароль в кодову панель - відкривається доступ до рубильника
  9. Потягнути за рубильник - перемикається УФ, доступ до коду в щоденнику
  10. Ввести код в лаб. пульт - запускається movie2.mp4 "Я - Білл сайфер", відкривається ніша з кнопками. Якщо кнопку не натиснули - грає movie3.mp4 "Скільки можна чекати"
  11. Натиснути кнопку "Відкрити" - Запускається  movie5.mp4 "Білла випустили", (перемикається музика 0004_Апокаліпсис.mp3), в автоматі теж грає відео, активується загадка "мемо".
  12. Розгадати "мемо" - відкривається ніша з підказкою і останньою частиною круга магічного
  13. Виставити магічний круг - таймер зупиняється, включається movie6.mp4 "Фінальне", грає фінальна музика 0005_Фінальна.mp3, двері відкриваються

  Якщо на пункті 11 - натиснути кнопку "Закрити портал" - грає відео movie4.mp4 "Ні - не робіть цього", кнопку "Відкрити" можна натиснути ще раз.
  Якщо натиснути "Закрити портал ще раз"- наступні пункти пропускаються, і зразу грає фінальна музика.

*/


#include <DFPlayer_Mini_Mp3.h>
#include <SoftwareSerial.h>

SoftwareSerial HC12(2, 3); // (Rx_pin, Tx_pin) //using a softwareSerial instead of serial, because of debugging through console and uploading a sketch
SoftwareSerial esp_wifi(44, 46); // (Rx_pin, Tx_pin) //using a softwareSerial instead of serial, because of debugging through console and uploading a sketch

//=======================MCS_IN-OUTS==============================
//========================MCS-left_OUTs
const int MOSF1_puchlya_door_D53 = 53; //MOSFET 1
const int MOSF2_exit_door_D51 = 51; //MOSFET 2
const int MOSF3_potolok_korobka_D49 = 49; //MOSFET 3
const int MOSF4_rubilnik_EML_D47 = 47; //MOSFET 4
const int MCS_D45 = 45; //MOSFET 5
const int MCS_D43 = 43; //MOSFET 6
const int MOSF7_table_D41 = 41; //MOSFET 7
const int MOSF8_window_D39 = 39; //MOSFET 8
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
#define MCS_quest_pls_5m_but_A4 A4
#define MCS_quest_start_but_A5 A5
#define MCS_quest_res_but_A6 A6
#define MCS_code_panel_NO_A7 A7 //ор. біл
#define MCS_rubilnik_A8 A8 //зел
#define MCS_A9 A9
#define MCS_A10 A10
#define MCS_A11 A11
#define MCS_A12 A12
#define MCS_A13 A13
#define MCS_A14 A14
#define MCS_A15 A15

//=======================MCS-up_an_INs

const int MCS_pukhlya_NO_40 = 40;
const int MCS_pukhlya_open_door_42 = 42;

const int REL_UV_D44 = 44;
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
String MCS_karp_cnfrm = "#SL_karp_done#"; //send command string should be "#xx...x#" format - for sure to correctly recieve a command. 1st "#" byte clears all junk before comparing, may work without it

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

String temp_string = ""; //variable to store information recieved form serial and compare it

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

bool dio = false;
bool karp_done = false;
bool telephone_done = false;
bool snack_automate_done = false; 


unsigned long time_quest_start_but = 0;

void setup() {
    Serial.begin(9600);             //UART
    Serial1.begin(9600); //pl1
    Serial2.begin(9600); //pl2
    Serial3.begin(9600); //HC12
    HC12.begin(9600); //

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
    pinMode(MCS_quest_pls_5m_but_A4, INPUT_PULLUP);
    pinMode(MCS_quest_start_but_A5, INPUT_PULLUP);
    pinMode(MCS_quest_res_but_A6, INPUT_PULLUP);
    pinMode(MCS_code_panel_NO_A7, INPUT_PULLUP); //ор.біл
    pinMode(MCS_rubilnik_A8, INPUT_PULLUP); //зел

    pinMode(MCS_pukhlya_NO_40, INPUT_PULLUP); //син+син.б.
    pinMode(MCS_pukhlya_open_door_42, INPUT_PULLUP); //зел+зел.б.
    
    pinMode(MOSF1_puchlya_door_D53, OUTPUT);
    pinMode(MOSF2_exit_door_D51, OUTPUT);
    pinMode(MOSF3_potolok_korobka_D49, OUTPUT);
    pinMode(MOSF4_rubilnik_EML_D47, OUTPUT);

    pinMode(MOSF7_table_D41, OUTPUT);
    pinMode(MOSF8_window_D39, OUTPUT);
    pinMode(REL_UV_D44, OUTPUT);

    digitalWrite(MOSF1_puchlya_door_D53, LOW);
    digitalWrite(MOSF2_exit_door_D51, HIGH);
    digitalWrite(MOSF3_potolok_korobka_D49, HIGH);
    digitalWrite(MOSF4_rubilnik_EML_D47, HIGH);
    digitalWrite(MOSF7_table_D41, HIGH);

    digitalWrite(REL_UV_D44, HIGH); //LOW = ON - UV light on

    

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
}

void posledovatelnost()
{
  //1. Адмін натискає старт - Стартує квест: таймер, запускається музика 0001_фонова_старт.mp3
  quest_start();
  //2. Пройти ігровий автомат, подивитися відео.
  //3. Виставити картини - відкриваєтсья ящик в потолку (підказка про пухлю)
  pic();
  //4. Посвітити в карпа - відкривається стол, отримуємо пухлю
  karp();
  //5. Прикласти пухлю - відкриваються двері (змінюється фонова музика 0002_кімната_Діпера.mp3 -якщо ми до цього не відкрили уже лабораторію), отримуємо щоденник
  pukhlya();
  //6. Подзвонити по телефону - вмикається відео з камер. Отримуємо код від снекового автомата
  telephone();
  //7. Ввести пароль в снек - запускається movie1.mp4 "Ефект порталу", відкриваються двері в лабораторію (змінюється фонова музика 0003_лабораторія.mp3),
  snack();  
  //8. Ввести пароль в кодову панель - відкривається доступ до рубильника
  code_panel();
  //9. Потягнути за рубильник - перемикається УФ, доступ до коду в щоденнику
  rubilnik();
  //10. Ввести код в лаб. пульт - запускається movie2.mp4 "Я - Білл сайфер", відкривається ніша з кнопками. Якщо кнопку не натиснули - грає movie3.mp4 "Скільки можна чекати"
  
  //11. Натиснути кнопку "Відкрити" - Запускається  movie5.mp4 "Білла випустили", (перемикається музика 0004_Апокаліпсис.mp3), в автоматі теж грає відео, активується загадка "мемо".
  //12. Розгадати "мемо" - відкривається ніша з підказкою і останньою частиною круга магічного
  //13. Виставити магічний круг - таймер зупиняється, включається movie6.mp4 "Фінальне", грає фінальна музика 0005_Фінальна.mp3, двері відкриваються


}

void quest_start()
{
  if(digitalRead(MCS_quest_start_but_A5) == LOW && dio == false){
    delay(50);
    Serial3.print(tmr_strt);
    mp3_set_serial(Serial1);
    mp3_play(1);
    dio = true;
  }
  else if(digitalRead(MCS_quest_start_but_A5) == HIGH && dio == true){delay(50);dio = false;}

  if(digitalRead(MCS_quest_res_but_A6) == LOW && dio == false){
    delay(50);
    Serial3.print(tmr_rst);
    mp3_set_serial(Serial1);
    mp3_stop();
    dio = true;
  }
  else if(digitalRead(MCS_quest_res_but_A6) == HIGH && dio == true){delay(50);dio = false;}

  if(digitalRead(MCS_quest_pls_5m_but_A4) == LOW && dio == false){
    delay(50);
    Serial3.print(tmr_pls_5m);
    dio = true;
  }
  else if(digitalRead(MCS_quest_pls_5m_but_A4) == HIGH && dio == true){delay(50);dio = false;}
}

void pic()
{
  int p1 = digitalRead(MCS_pict1_A0);
  int p2 = digitalRead(MCS_pict2_A1);
  int p3 = digitalRead(MCS_pict3_A2);
  int p4 = digitalRead(MCS_pict4_A3);
  if(p1==1 && p2==1 && p3==1 && p4==1)
  {
    digitalWrite(MOSF3_potolok_korobka_D49, LOW);
  }

  else if(p1==0 || p2==0 || p3==0 || p4==0)
  {
    digitalWrite(MOSF3_potolok_korobka_D49, HIGH);
  }
  /*Serial3.print(p1);
  Serial3.print(" ");
  Serial3.print(p2);
  Serial3.print(" ");
  Serial3.print(p3);
  Serial3.print(" ");
  Serial3.println(p4);
  delay(500);*/
}

void karp()
{
  if(karp_done)
  {
    digitalWrite(MOSF7_table_D41, HIGH);
    karp_done=!karp_done;
  }
}

void pukhlya()
{
  if(digitalRead(MCS_pukhlya_NO_40) == LOW){
    delay(50); 
    digitalWrite(MOSF1_puchlya_door_D53, LOW);
    delay(500); 
    Serial3.println("pu_op");
  }
  else if(digitalRead(MCS_pukhlya_NO_40) == HIGH){delay(50); digitalWrite(MOSF1_puchlya_door_D53, HIGH);}
}

void telephone()
{
  if(telephone_done)
  {
    digitalWrite(MCS_D30, LOW); //5v TV led on pin
    telephone_done=!telephone_done;
  }
}

void snack()
{
  if(snack_automate_done)
  {
    mp3_set_serial(Serial1);
    mp3_play(3);
    snack_automate_done=!snack_automate_done;
  }
}

void code_panel()
{
  if(digitalRead(MCS_code_panel_NO_A7) == LOW)
  {
    delay(50); 
    digitalWrite(MOSF4_rubilnik_EML_D47, LOW);
  }

  else if(digitalRead(MCS_code_panel_NO_A7) == HIGH){delay(50); digitalWrite(MOSF4_rubilnik_EML_D47, HIGH);}
}

void rubilnik()
{
  if(digitalRead(MCS_rubilnik_A8) == LOW){
    delay(50); 
    digitalWrite(REL_UV_D44, LOW);
  }
  else if(digitalRead(MCS_rubilnik_A8) == HIGH){delay(50); digitalWrite(REL_UV_D44, HIGH);}
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
        Serial3.print(tmr_strt);
        mp3_set_serial(Serial1);
        mp3_play(1);
      }

      if (temp_string == MCS_karp_cnfrm)  //compare string with a known commands
      {
        karp_done = true;
      }

      if (temp_string == cnfrm_TV)  //compare string with a known commands
      {
        telephone_done = true;
      }

      if (temp_string == snack_done)  //compare string with a known commands
      {
        snack_automate_done = true;
      }

      

      temp_string = "";     //then clear the string
    }
  }
}
