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


/*what to do:
+ Перезагрузка
  + Телефон підстроїти під перезагрузку + гучність
  + Ігрова консоль - підстроїти під перезагрузку
+ Фінал - підлаштувати
  Надо будет заполнить такую логику:
  - собрали круг
  - ТАЙМЕР СТОП
  - одновременно из колонок громко голос младенца времени с призывом подойти к порталу
  - только после этого (т.е. секунд через 7-10 как сложили круг) запуск финального видео. При этом, фоновой музыки быть не должно, пока не появится статичная картинка.
  - как только в видео пошла статичная картинка, запускается фоновая музыка - ремикс на заглавную тему и открывается входная дверь

- Музика - підлаштувати по таймеру
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
const int MOSF5_lab_door_EML_D45 = 45; //MOSFET 5
const int MCS_D43 = 43; //MOSFET 6
const int MOSF7_table_D41 = 41; //MOSFET 7
const int MOSF8_window_D39 = 39; //MOSFET 8
const int MCS_snack_done_D37 = 37; //
const int MCS_magic_circle_done_D35 = 35; //
const int MCS_TV_survelliance_5v_en_D33 = 33; //
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
const int REL_LIGHT_D46 = 46;
const int REL_RPI_5V = 48;
const int REL_RESET_D50 = 50;

const int MCS_D52 = 52;

String string;
String string_reply;

//GF_Game_console
String play_vid_1 = "play_vid_1#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String play_vid_2 = "play_vid_2#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String play_vid_3 = "play_vid_3#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String pleer_on = "pleer_on#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String pleer_off = "pleer_off#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String reset_cons = "reset_cons#";
String music_low = "music_low#";
String music_high = "music_high#";

//GF_karp_nolib
String MCS_karp_play = "krp_pl#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String MCS_karp_cnfrm = "krp_d#"; //send command string should be "#xx...x#" format - for sure to correctly recieve a command. 1st "#" byte clears all junk before comparing, may work without it

//GF_lab_panel
//Slave-Master strings
String but21_done = "but21_done#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String open_port = "open_port#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String try_close_port = "try_close_port#";
String close_port = "close_port#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String nt_op_port = "nt_op_port#";
String but9_done = "but9_done#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String por_tim_ov = "por_tim_ov#";

//Master-Slave strings
String but21_open = "but21_op#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String but_op_portal = "but_op_portal#"; // emulating opening portal
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
String cnfrm_TV = "SL_TV_done#"; //send command string should be "#xx...x#" format - for sure to correctly recieve a command. 1st "#" byte clears all junk before comparing, may work without it
String Tel_res = "Tel_res#";

//GF_Teleport strings
String Tele_mov1 = "#Tele_mov1#"; //portal endless galactic
String Tele_mov2 = "#Tele_mov2#"; //Я - Білл сайфер
String Tele_mov3 = "#Tele_mov3#"; //Скільки можна чекати
String Tele_mov4 = "#Tele_mov4#"; //Ні - не робіть цього
String Tele_mov5 = "#Tele_mov5#"; //Білла випустили
String Tele_mov6 = "#Tele_mov6#"; //Фінальне
String Tele_on = "#Tele_on#"; //compare string should be "xx...x#" format. Last "#" sign is a stop byte
String Vid_play = "Vid_play#";

//GF_timer_max7219
//Slave-Master strings
String time_is_over = "tm_ovr#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
//Master-Slave strings
String tmr_strt = "tmr_strt#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String tmr_pls_5m = "tmr_pls_5m#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String tmr_rst = "tmr_rst#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte
String tmr_stp = "tmr_stp#"; //compared string should be "xx...x#" format. Last "#" sign is a stop byte

String load_all = "load_all#"; // reload EPS_8266_WEB buttons

String temp_string = ""; //variable to store information recieved form serial and compare it

String test = "test#";

/*/GF_remote_XY
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
};*/

//Admin_WEB_commands
String WEB1_cons = "WEB1#"; //play_vid_1#
String WEB2_karp = "WEB2#"; //krp_pl#
String WEB3_pict = "WEB3#";
String WEB4_pukh = "WEB4#";
String WEB5_phon = "WEB5#"; //MCS_TV_play#
String WEB6_snac = "WEB6#"; //open_sn#
String WEB7_code = "WEB7#";
String WEB8_21bu = "WEB8#"; //but21_op# //not working
String WEB9_open = "WEB9#"; //but_op_portal#
String WEB10_9but = "WEB10#"; //but9_op#
String WEB11_magi = "WEB11#"; //
String WEB12 = "WEB12#";


bool dio = false;

bool flag_is_started = false;
bool flag_karp_done = false;
bool flag_telephone_done = false;
bool is_pukhlya = false;
bool flag_snack_automate_done = false;
bool is_code_panel = false;
bool is_rubilnik = false;
bool flag_but21_done = false;
bool flag_open_port = false;
bool flag_close_port = false;
bool flag_but9_done = false;
bool flag_try_close_port = false;
bool flag_por_tim_ov = false;
bool flag_nt_op_port = false;
bool flag_circle_done = false;
bool flag_game_over = false;
bool is_portal_playing = false;

bool flag_is_reset = false;

bool flag1_song = false;
bool flag2_song = false;
bool flag3_song = false;
bool flag4_song = false;

unsigned long time_quest_start_but = 0;

void setup() {
    Serial.begin(9600);             //UART
    Serial1.begin(9600); //pl1 music
    Serial2.begin(9600); //pl2 effects
    Serial3.begin(9600); //HC12
    HC12.begin(9600); //

    mp3_set_serial(Serial1);
    delay(30);
    mp3_set_volume (28);
    delay(30);
    mp3_set_serial(Serial2);
    delay(30);
    mp3_set_volume (28);

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
    pinMode(MOSF5_lab_door_EML_D45, OUTPUT);

    pinMode(MOSF7_table_D41, OUTPUT);
    pinMode(MOSF8_window_D39, OUTPUT);
    pinMode(MCS_snack_done_D37, INPUT_PULLUP); //біл
    pinMode(MCS_magic_circle_done_D35, INPUT_PULLUP);//зел   
    pinMode(MCS_TV_survelliance_5v_en_D33, OUTPUT);

    pinMode(REL_UV_D44, OUTPUT);
    pinMode(REL_LIGHT_D46, OUTPUT); //NORMAL CLOSED CONTACT
    pinMode(REL_RPI_5V, OUTPUT); //
    pinMode(REL_RESET_D50, OUTPUT);//

    digitalWrite(MOSF1_puchlya_door_D53, HIGH);
    digitalWrite(MOSF2_exit_door_D51, HIGH);
    digitalWrite(MOSF3_potolok_korobka_D49, HIGH);
    digitalWrite(MOSF4_rubilnik_EML_D47, HIGH); 
    digitalWrite(MOSF5_lab_door_EML_D45, HIGH);

    digitalWrite(MOSF7_table_D41, HIGH);
    digitalWrite(MOSF8_window_D39, HIGH);
    digitalWrite(MCS_TV_survelliance_5v_en_D33, HIGH);

    digitalWrite(REL_UV_D44, HIGH); //LOW = ON - UV light on
    digitalWrite(REL_LIGHT_D46, HIGH); //LOW = ON - main light on //NORMAL CLOSED CONTACT
    digitalWrite(REL_RPI_5V, HIGH); //LOW = ON - main light on //NORMAL CLOSED CONTACT
    digitalWrite(REL_UV_D44, LOW); //LOW = ON

    delay(15000);
    digitalWrite(REL_RPI_5V, LOW); //LOW = ON
    delay(20000);
    digitalWrite(MCS_TV_survelliance_5v_en_D33, LOW);

  Serial3.print(load_all);    
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
  posledovatelnost();
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
  lab_panel();
  //11. Скласти магічний круг
  magic_circle();
  //якщо час закінчився
  game_over();
  //ф-ція для перезавантаження
  reset_state();
}

void quest_start()
{
  //start button
  if(digitalRead(MCS_quest_start_but_A5) == LOW && flag_is_started == false){
    delay(50);
    Serial3.print(tmr_strt);
    delay(50);
    mp3_set_serial(Serial1);
    delay(50);
    mp3_play(1);
    flag_is_started = true;

    song();
  }
  //else if(digitalRead(MCS_quest_start_but_A5) == HIGH && flag_is_started == true){delay(50);dio = flag_is_started;}

  //reset button
  if(digitalRead(MCS_quest_res_but_A6) == LOW && dio == false){
    delay(50);
    
    dio = true;
    flag_is_started = false;
    flag_is_reset = true;

    song();
  }
  else if(digitalRead(MCS_quest_res_but_A6) == HIGH && dio == true){delay(50);dio = false;}

  //pls 5m button
  if(digitalRead(MCS_quest_pls_5m_but_A4) == LOW && dio == false){
    delay(200);
    Serial3.print(tmr_pls_5m);
    dio = true;

    song();
  }
  else if(digitalRead(MCS_quest_pls_5m_but_A4) == HIGH && dio == true){delay(50);dio = false;}
}

void pic()
{
  int p1 = digitalRead(MCS_pict1_A0);
  int p2 = digitalRead(MCS_pict2_A1);
  int p3 = digitalRead(MCS_pict3_A2);
  int p4 = digitalRead(MCS_pict4_A3);
  if(p1==0 && p2==0 && p3==0 && p4==0)
  {
    digitalWrite(MOSF3_potolok_korobka_D49, LOW);
    delay(500);
  }

  else if(p1==1 || p2==1 || p3==1 || p4==1)
  {
    digitalWrite(MOSF3_potolok_korobka_D49, HIGH);
  }

  //code to check pictures
  /*if(p1==0)
  {
    flag1_song=true;
    if(flag1_song) song();
  }
  else flag1_song=false;

  if(p2==0)
  {
    flag2_song=true;
    if(flag2_song) song();
  }
  else flag2_song=false;

  if(p3==0)
  {
    flag3_song=true;
    if(flag3_song) song();
  }
  else flag3_song=false;

  if(p4==0)
  {
    flag4_song=true;
    if(flag4_song) song();
  }
  else flag4_song=false;*/


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
  if(flag_karp_done)
  {
    digitalWrite(MOSF7_table_D41, LOW);
    flag_karp_done=false;
  }
}

void pukhlya()
{
  if(digitalRead(MCS_pukhlya_NO_40) == LOW && is_pukhlya == false){
    delay(50); 
    digitalWrite(MOSF1_puchlya_door_D53, LOW);
    delay(500); 
    song();
    is_pukhlya = true;

  }
  else if(is_pukhlya == false){}//to not close doors while WEB4_pukh
  else if(digitalRead(MCS_pukhlya_NO_40) == HIGH){delay(50); digitalWrite(MOSF1_puchlya_door_D53, HIGH);is_pukhlya = false;}
}

void telephone()
{
  if(flag_telephone_done)
  {
    digitalWrite(MCS_TV_survelliance_5v_en_D33, HIGH); //5v TV led on pin
    Serial3.print(activate_snack);
    flag_telephone_done=!flag_telephone_done;
  }
}

void snack()
{ 
  if (digitalRead(MCS_snack_done_D37) == LOW && flag_snack_automate_done == false)
  {
    delay(50);
    flag_snack_automate_done = true;
  }

  if(flag_snack_automate_done)
  {
    mp3_set_serial(Serial1);
    delay(10);    mp3_set_volume(28);
    delay(50);    mp3_play(2); //laboratory music

    digitalWrite(MOSF5_lab_door_EML_D45, LOW); //open lab door

    Serial3.print(Tele_mov1); //play video 1 - "Endless Galactic" at portal
    flag_snack_automate_done=!flag_snack_automate_done;
  }
}

void code_panel()
{
  if(digitalRead(MCS_code_panel_NO_A7) == LOW && is_code_panel == false)
  {
    delay(50); 
    digitalWrite(MOSF4_rubilnik_EML_D47, LOW);
    song();
    is_code_panel = true;
  }
  else if(is_code_panel==false){}
  else if(digitalRead(MCS_code_panel_NO_A7) == HIGH){delay(50); digitalWrite(MOSF4_rubilnik_EML_D47, HIGH);is_code_panel = false;}
}

void rubilnik()
{
  if(digitalRead(MCS_rubilnik_A8) == LOW && is_rubilnik == false){
    delay(50); 
    digitalWrite(REL_UV_D44, LOW);
    digitalWrite(REL_LIGHT_D46, LOW);
    song();
    is_rubilnik = true;
  }
  else if(digitalRead(MCS_rubilnik_A8) == HIGH){delay(50); digitalWrite(REL_UV_D44, HIGH);digitalWrite(REL_LIGHT_D46, HIGH);is_rubilnik = false;}
}

void lab_panel()
{
  if(flag_but21_done)
  {
    Serial3.print(Tele_mov2); //play video 2 - "О, привіт" at portal
    delay(50);
    mp3_set_serial(Serial1);
    mp3_set_volume(20);
    flag_but21_done=false;
  }

  if(flag_nt_op_port)
  {
    mp3_set_serial(Serial1);
    delay(10);    mp3_set_volume(15);
    Serial3.print(Tele_mov3); //play video 3 - "Скільки можна чекати" at portal
    flag_nt_op_port=false;
  }

  if(flag_try_close_port)
  {
    Serial3.print(Tele_mov4); //play video 4 - "Не робіть цього" at portal
    flag_try_close_port=false;
  }

  if(flag_open_port) //apocalypse
  {
    Serial3.print(Tele_mov5); //play video 5 - "Які ж ви довірливі" at portal
    delay(90000);
    mp3_set_serial(Serial1);
    delay(10);    mp3_set_volume(3);
    mp3_set_serial(Serial2);
    delay(10);    mp3_set_volume(25);
    delay(10);    mp3_play(51); //що ж ви накоїли
    delay(20000);
    mp3_set_serial(Serial1);
    delay(10);    mp3_set_volume(25);
    delay(10);    mp3_play(3); //apocalypse music
    flag_open_port=false;
  }

  if(flag_close_port)
  {
    mp3_set_serial(Serial1);
    delay(10);    mp3_set_volume(5);

    Serial3.print(Tele_mov6); //play video 6 - "Фіналочка" at portal
    delay(50); 
    Serial3.print(tmr_stp);
    flag_close_port=false;

    digitalWrite(MOSF2_exit_door_D51, LOW); //open doors
    digitalWrite(REL_UV_D44, HIGH); //LOW = ON - UV light on
    digitalWrite(REL_LIGHT_D46, HIGH); //LOW = ON - main light on //NORMAL CLOSED CONTACT
    delay(85000);
    mp3_set_serial(Serial1);
    delay(10);    mp3_set_volume(30);
    delay(10);    mp3_play(4); //win music
  }

  if(flag_por_tim_ov)
  {
    flag_close_port=false;

    mp3_set_serial(Serial1);
    delay(10);    mp3_set_volume(5);

    Serial3.print(tmr_stp);
    digitalWrite(MOSF2_exit_door_D51, LOW); //open doors
    digitalWrite(REL_UV_D44, HIGH); //LOW = ON - UV light on
    digitalWrite(REL_LIGHT_D46, HIGH); //LOW = ON - main light on //NORMAL CLOSED CONTACT

    delay(65000);
    mp3_set_serial(Serial1);
    delay(10);    mp3_set_volume(28);
    delay(10);    mp3_play(4); //win music
    flag_por_tim_ov = false;
  }
}

void magic_circle()
{ 
  if(digitalRead(MCS_magic_circle_done_D35) == LOW)
  {
    flag_circle_done=true;
    Serial.println("circle_done_analogue");
  }

  if(flag_circle_done)
  {
    Serial.println("circle_done");

    Serial3.print(tmr_stp);

    mp3_set_serial(Serial1);
    delay(10);     mp3_set_volume(5);
    mp3_set_serial(Serial2);
    delay(10);     mp3_set_volume(30);
    delay(10);     mp3_play(52); // "мерщій сюди"
    delay(7000);

    Serial3.print(Tele_mov6); //play video 6 - "Фіналочка" at portal
    delay(50); 

    flag_close_port=false;

    digitalWrite(MOSF2_exit_door_D51, LOW); //open doors
    digitalWrite(REL_UV_D44, HIGH); //LOW = ON - UV light on
    digitalWrite(REL_LIGHT_D46, HIGH); //LOW = ON - main light on //NORMAL CLOSED CONTACT
   
    delay(92000);
    mp3_set_serial(Serial1);
    delay(10);    mp3_set_volume(30);
    delay(50);    mp3_play(4); //win music
    flag_circle_done=false;
  }
}

void game_over()
{
  if(flag_game_over)
  {
    digitalWrite(MOSF2_exit_door_D51, LOW); //open doors
    digitalWrite(REL_UV_D44, HIGH); //LOW = ON - UV light on
    digitalWrite(REL_LIGHT_D46, HIGH); //LOW = ON - main light on //NORMAL CLOSED CONTACT
    mp3_set_serial(Serial2);
    delay(10);    mp3_set_volume(25);
    delay(50);    mp3_play(6); //loose music effect

    mp3_set_serial(Serial1);
    delay(10);    mp3_set_volume(25);
    delay(50);    mp3_play(5); //loose music
    flag_game_over=false;
  }
}

void reset_state()
{
  if(flag_is_reset)
  {
    digitalWrite(MOSF1_puchlya_door_D53, HIGH);
    digitalWrite(MOSF2_exit_door_D51, HIGH);
    digitalWrite(MOSF3_potolok_korobka_D49, HIGH);
    digitalWrite(MOSF4_rubilnik_EML_D47, HIGH);
    digitalWrite(MOSF5_lab_door_EML_D45, HIGH);
    digitalWrite(MOSF7_table_D41, HIGH);
    digitalWrite(MOSF8_window_D39, HIGH);
    digitalWrite(MCS_TV_survelliance_5v_en_D33, HIGH);

    digitalWrite(REL_UV_D44, HIGH); //LOW = ON - UV light on
    digitalWrite(REL_LIGHT_D46, HIGH); //LOW = ON - main light on //NORMAL CLOSED CONTACT
    digitalWrite(REL_RPI_5V, LOW); //LOW = ON - main light on //NORMAL CLOSED CONTACT
    digitalWrite(REL_RESET_D50, HIGH);

    Serial3.print(tmr_rst);
    delay(10);  
    mp3_set_serial(Serial1);
    delay(50);  
    mp3_pause();
    delay(10);
    Serial3.print(reset_cons);
    delay(500);
    Serial3.print(reset_snack);
    delay(500);
    Serial3.print(reset_lab_panel);
    delay(500);
    Serial3.print(Tel_res);
    delay(50000);
    digitalWrite(MCS_TV_survelliance_5v_en_D33, LOW);

    flag_is_reset = false;
  }

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
      //Serial.print(temp_string);
      //Serial3.println("MCS recieved#");

      if (temp_string == MCS_karp_cnfrm){flag_karp_done = true; song();}
      if (temp_string == cnfrm_TV){flag_telephone_done = true; song();}
      if (temp_string == snack_done){flag_snack_automate_done = true; song();}
      if (temp_string == but21_done){flag_but21_done = true; song();}
      if (temp_string == nt_op_port){flag_nt_op_port = true; song();}
      if (temp_string == try_close_port){flag_try_close_port = true; song();}
      if (temp_string == por_tim_ov){flag_por_tim_ov = true; song();}
      if (temp_string == open_port){flag_open_port = true; song();}
      if (temp_string == close_port){flag_close_port = true; song();}
      if (temp_string == but9_done){flag_but9_done = true; song();}
      if (temp_string == mgc_crcl_done){flag_circle_done = true; song();}
      if (temp_string == time_is_over){flag_game_over = true;}

      if (temp_string == music_low){mp3_set_serial(Serial1);mp3_set_volume(15);}
      if (temp_string == music_high){mp3_set_serial(Serial1);mp3_set_volume(28);}
      if (temp_string == test){song();}
      if (temp_string == Vid_play) {is_portal_playing=true;}

      //WEB_interface_commands
      if (temp_string == WEB1_cons){delay(100); Serial3.print(play_vid_1);song();}
      if (temp_string == WEB2_karp){delay(100); flag_karp_done=true;Serial3.print(MCS_karp_play); song();}
      if (temp_string == WEB3_pict){delay(100); digitalWrite(MOSF3_potolok_korobka_D49, LOW); delay(500);song();}
      if (temp_string == WEB4_pukh){delay(100); digitalWrite(MOSF1_puchlya_door_D53, LOW);delay(500);song();} //need to check
      if (temp_string == WEB5_phon){delay(100); flag_telephone_done=true; Serial3.print(MCS_TV_play); song();}
      if (temp_string == WEB6_snac){delay(100); flag_snack_automate_done = true; song();}
      if (temp_string == WEB7_code){delay(100); digitalWrite(MOSF4_rubilnik_EML_D47, LOW); delay(500); song();}
      if (temp_string == WEB8_21bu){delay(100); flag_but21_done=true; Serial3.print(but21_open); song();}
      if (temp_string == WEB9_open){delay(100); flag_open_port=true; Serial3.print(but_op_portal); song();} //not working
      if (temp_string == WEB10_9but){delay(100); Serial3.print(but9_open); song();} //not working
      if (temp_string == WEB11_magi){delay(100); flag_circle_done=true; song();} //not working
      if (temp_string == WEB12){song();}
      temp_string = "";     //then clear the string
    }
  }
}

void song()
{
  mp3_set_serial(Serial2);
  delay(10);  mp3_play(9);
}