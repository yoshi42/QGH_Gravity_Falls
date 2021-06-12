#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>

SoftwareSerial Serial_HC(D1, D2); // (Rx_pin, Tx_pin) //using a softwareSerial instead of serial, because of debugging through console and uploading a sketch

/* Установите здесь свои SSID и пароль */
const char* ssid = "avocado";  // SSID
const char* password = "greengreen37"; // пароль

ESP8266WebServer server(80);

uint8_t LED1pin = D7;
uint8_t LED2pin = D6;
uint8_t LED3pin = D8;
uint8_t LED4pin = D8;
uint8_t LED5pin = D8;
uint8_t LED6pin = D8;
uint8_t LED7pin = D8;
uint8_t LED8pin = D8;
uint8_t LED9pin = D8;
uint8_t LED10pin = D8;

bool LED1status = false;
bool LED2status = false;
bool LED3status = false;
bool LED4status = false;
bool LED5status = false;
bool LED6status = false;
bool LED7status = false;
bool LED8status = false;
bool LED9status = false;
bool LED10status = false;

const byte num_load = 10;
String load[num_load] = 
{ "load1#", 
  "load2#", 
  "load3#", 
  "load4#",
  "load5#",
  "load6#", 
  "load7#", 
  "load8#", 
  "load9#",
  "load10#",
};

String load_all = "load_all#";

String done[num_load] = 
{ "done1#",
  "done2#",
  "done3#",
  "done4#",
  "done5#",
  "done6#",
  "done7#",
  "done8#",
  "done9#",
  "done10#",
};

String temp_string = "";

void setup() 
{
  Serial.begin(9600);
  Serial_HC.begin(9600);
  delay(100);
  pinMode(LED1pin, OUTPUT);
  pinMode(LED2pin, OUTPUT);

  Serial.println("Connecting to ");
  Serial.println(ssid);

  // подключиться к вашей локальной wi-fi сети
  WiFi.begin(ssid, password);

  // проверить, подключился ли wi-fi модуль к wi-fi сети
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.on("/refresh", handle_refresh);
  server.on("/led1on", handle_led1on);
  server.on("/led2on", handle_led2on);
  server.on("/led3on", handle_led3on);
  server.on("/led4on", handle_led4on);
  server.on("/led5on", handle_led5on);
  server.on("/led6on", handle_led6on);
  server.on("/led7on", handle_led7on);
  server.on("/led8on", handle_led8on);
  server.on("/led9on", handle_led9on);
  server.on("/led10on", handle_led10on);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() 
{
  server.handleClient();
  HC12_loop();
  
  if(LED1status)
    digitalWrite(LED1pin, HIGH);
  else
    digitalWrite(LED1pin, LOW);
  
  if(LED2status)
    digitalWrite(LED2pin, HIGH);
  else
    digitalWrite(LED2pin, LOW);
}

void handle_NotFound(){  server.send(404, "text/plain", "Not found");
}

void handle_OnConnect() 
{
  server.send(200, "text/html", SendHTML(LED1status,LED2status,LED3status,LED4status,LED5status,LED6status,LED7status,LED8status,LED9status,LED10status));
}
void handle_refresh(){ Serial.println("refreshed");  server.send(200, "text/html", SendHTML(LED1status,LED2status,LED3status,LED4status,LED5status,LED6status,LED7status,LED8status,LED9status,LED10status));}
void handle_led1on(){   LED1status = true;  Serial_HC.print("b1#");  server.send(200, "text/html", SendHTML(true,LED2status,LED3status,LED4status,LED5status,LED6status,LED7status,LED8status,LED9status,LED10status));}
void handle_led2on(){   LED2status = true;  Serial_HC.print("b2#");  server.send(200, "text/html", SendHTML(LED1status,true,LED3status,LED4status,LED5status,LED6status,LED7status,LED8status,LED9status,LED10status)); }
void handle_led3on(){   LED3status = true;  Serial_HC.print("b3#");  server.send(200, "text/html", SendHTML(LED1status,LED2status,true,LED4status,LED5status,LED6status,LED7status,LED8status,LED9status,LED10status)); }
void handle_led4on(){   LED4status = true;  Serial_HC.print("b4#");  server.send(200, "text/html", SendHTML(LED1status,LED2status,LED3status,true,LED5status,LED6status,LED7status,LED8status,LED9status,LED10status)); }
void handle_led5on(){   LED5status = true;  Serial_HC.print("b5#");  server.send(200, "text/html", SendHTML(LED1status,LED2status,LED3status,LED4status,true,LED6status,LED7status,LED8status,LED9status,LED10status)); }
void handle_led6on(){   LED6status = true;  Serial_HC.print("b6#");  server.send(200, "text/html", SendHTML(LED1status,LED2status,LED3status,LED4status,LED5status,true,LED7status,LED8status,LED9status,LED10status)); }
void handle_led7on(){   LED7status = true;  Serial_HC.print("b7#");  server.send(200, "text/html", SendHTML(LED1status,LED2status,LED3status,LED4status,LED5status,LED6status,true,LED8status,LED9status,LED10status)); }
void handle_led8on(){   LED8status = true;  Serial_HC.print("b8#");  server.send(200, "text/html", SendHTML(LED1status,LED2status,LED3status,LED4status,LED5status,LED6status,LED7status,true,LED9status,LED10status)); }
void handle_led9on(){   LED9status = true;  Serial_HC.print("b9#");  server.send(200, "text/html", SendHTML(LED1status,LED2status,LED3status,LED4status,LED5status,LED6status,LED7status,LED8status,true,LED10status)); }
void handle_led10on(){  LED10status = true;  Serial_HC.print("b10#");  server.send(200, "text/html", SendHTML(LED1status,LED2status,LED3status,LED4status,LED5status,LED6status,LED7status,LED8status,LED9status,true)); }

String SendHTML(uint8_t led1stat,uint8_t led2stat,uint8_t led3stat,uint8_t led4stat,uint8_t led5stat,uint8_t led6stat,uint8_t led7stat,uint8_t led8stat,uint8_t led9stat,uint8_t led10stat)
{
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Gravity Falls Quest</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 10px;} h1 {color: #444444;margin: 20px auto 20px;} h3 {color: #444444;margin-bottom: 10px;}\n";
  ptr +=".button {display: block;width: 300px;background-color: #1abc9c;border: none;color: white;padding: 13px 20px;text-decoration: none;font-size: 25px;margin: 0px auto 8px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-light {background-color: #1abc9c;}\n";
  ptr +=".button-light:active {background-color: #16a085;}\n";
  ptr +=".button-dark {background-color: #34495e;}\n";
  ptr +=".button-dark:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 20px;color: #888;margin-bottom: 5px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +=" <h2><a href=\"/refresh\">Gravity Falls Quest Room</a></h1>";
  ptr +="<h3>The House</h3>\n";
  
  if(!led1stat)
    ptr +="<p>quest 1: loaded</p><a class=\"button button-light\" href=\"/led1on\">quest 1</a>\n";
  else
    ptr +="<p>quest 1: done</p><a class=\"button button-dark\" href=\"/led1on\">quest 1</a>\n";

  if(!led2stat)
    ptr +="<p>quest 2: loaded</p><a class=\"button button-light\" href=\"/led2on\">quest 2</a>\n";
  else
    ptr +="<p>quest 2: done</p><a class=\"button button-dark\" href=\"/led2on\">quest 2</a>\n";

  if(!led3stat)
    ptr +="<p>quest 3: loaded</p><a class=\"button button-light\" href=\"/led3on\">quest 3</a>\n";
  else
    ptr +="<p>quest 3: done</p><a class=\"button button-dark\" href=\"/led3on\">quest 3</a>\n";

  if(!led4stat)
    ptr +="<p>quest 4: loaded</p><a class=\"button button-light\" href=\"/led4on\">quest 4</a>\n";
  else
    ptr +="<p>quest 4: done</p><a class=\"button button-dark\" href=\"/led4on\">quest 4</a>\n";

  if(!led5stat)
    ptr +="<p>quest 5: loaded</p><a class=\"button button-light\" href=\"/led5on\">quest 5</a>\n";
  else
    ptr +="<p>quest 5: done</p><a class=\"button button-dark\" href=\"/led5on\">quest 5</a>\n";

  if(!led6stat)
    ptr +="<p>quest 6: loaded</p><a class=\"button button-light\" href=\"/led6on\">quest 6</a>\n";
  else
    ptr +="<p>quest 6: done</p><a class=\"button button-dark\" href=\"/led6on\">quest 6</a>\n";

  if(!led7stat)
    ptr +="<p>quest 7: loaded</p><a class=\"button button-light\" href=\"/led7on\">quest 7</a>\n";
  else
    ptr +="<p>quest 7: done</p><a class=\"button button-dark\" href=\"/led7on\">quest 7</a>\n";

  if(!led8stat)
    ptr +="<p>quest 8: loaded</p><a class=\"button button-light\" href=\"/led8on\">quest 8</a>\n";
  else
    ptr +="<p>quest 8: done</p><a class=\"button button-dark\" href=\"/led8on\">quest 8</a>\n";

  if(!led9stat)
    ptr +="<p>quest 9: loaded</p><a class=\"button button-light\" href=\"/led9on\">quest 9</a>\n";
  else
    ptr +="<p>quest 9: done</p><a class=\"button button-dark\" href=\"/led9on\">quest 9</a>\n";

  if(!led10stat)
    ptr +="<p>quest 10: loaded</p><a class=\"button button-light\" href=\"/led10on\">quest 10</a>\n";
  else
    ptr +="<p>quest 10: done</p><a class=\"button button-dark\" href=\"/led10on\">quest 10</a>\n";

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

void HC12_loop()
{              //recieve something from hc-12 inerface
  while (Serial.available())
  {
    char inChar = Serial.read(); //store each bite in var
    temp_string += inChar;     //add to string
    Serial_HC.print(inChar); //Send each recieved byte back
    
  if (inChar == '#')       //if stop byte recieved
    {
      //Serial.print(temp_string);
      if (temp_string == load[0]) {Serial.println(load[0]); LED1status=false;}
      if (temp_string == load[1]) {Serial.println(load[1]); LED2status=false;}
      if (temp_string == load[2]) {Serial.println(load[2]); LED3status=false;}
      if (temp_string == load[3]) {Serial.println(load[3]); LED4status=false;}
      if (temp_string == load[4]) {Serial.println(load[4]); LED5status=false;}
      if (temp_string == load[5]) {Serial.println(load[5]); LED6status=false;}
      if (temp_string == load[6]) {Serial.println(load[6]); LED7status=false;}
      if (temp_string == load[7]) {Serial.println(load[7]); LED8status=false;}
      if (temp_string == load[8]) {Serial.println(load[8]); LED9status=false;}
      if (temp_string == load[9]) {Serial.println(load[9]); LED10status=false;}
      
      if (temp_string == load_all) 
      {
        Serial.println(load_all); 
        LED1status=false;
        LED2status=false;
        LED3status=false;
        LED4status=false;
        LED5status=false;
        LED6status=false;
        LED7status=false;
        LED8status=false;
        LED9status=false;
        LED10status=false;
      }

      if (temp_string == done[0]) {Serial.println(done[0]); LED1status=true;}
      if (temp_string == done[1]) {Serial.println(done[1]); LED2status=true;}
      if (temp_string == done[2]) {Serial.println(done[2]); LED3status=true;}
      if (temp_string == done[3]) {Serial.println(done[3]); LED4status=true;}
      if (temp_string == done[4]) {Serial.println(done[4]); LED5status=true;}
      if (temp_string == done[5]) {Serial.println(done[5]); LED6status=true;}
      if (temp_string == done[6]) {Serial.println(done[6]); LED7status=true;}
      if (temp_string == done[7]) {Serial.println(done[7]); LED8status=true;}
      if (temp_string == done[8]) {Serial.println(done[8]); LED9status=true;}
      if (temp_string == done[9]) {Serial.println(done[9]); LED10status=true;}

      temp_string = "";     //then clear the string
    }
  }
}