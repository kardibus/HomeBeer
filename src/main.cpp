#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "EEPROM.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_SSD1306.h>

// Вставьте ниже SSID и пароль для своей WiFi-сети:
const char* ssid = "EasyBear";
const char* password = "12345678";
String p;
String s;
boolean st;
const char* pp = p.c_str();
const char* ss = s.c_str();


#define EEPROM_SIZE 0
// Задаем GPIO-контакт, к которому подключен светодиод:
const int ledPin = 2;
//String isCHeck = "STA";

AsyncWebServer server(80);

// Меняем заглушку на текущее состояние светодиода:
String processor(const String& var){
  // Создаем переменную для хранения состояния светодиода:
  String ledState;

  if(var == "STATE"){
    if(digitalRead(ledPin)){
      ledState = "ON";
    }
    else{
      ledState = "OFF";
    }
    Serial.println(ledState);
    return ledState;
  }
  return String();
}
 
String stateNet(const String& var){
  if (var == "STATENET"){
   if(st){
       return "AP";
    }
  return "STA";
}
 return String();
}

Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup(){
   if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
  }
 EEPROM.begin(200);

 p = EEPROM.readString(0);
 s = EEPROM.readString(30);
 st = EEPROM.readBool(60);

  // Включаем последовательную коммуникацию (для отладки):
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  // Инициализируем SPIFFS:
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
               //  "При монтировании SPIFFS произошла ошибка"
    return;
  }
 
   Serial.println(ss);
       Serial.println(pp);
         Serial.println(st);
 
WiFi.disconnect();

if(st == 1){
  WiFi.softAP(ssid,password);
    IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  display.clearDisplay();
    display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("AP IP address:");
  display.println(IP);
  display.display();
}
 if(st == 0){
  WiFi.begin(ss,pp);
  while (WiFi.status() !=  WL_CONNECTED)
  {
    Serial.println("Wifi connected.....");
    delay(10000);
    while (WiFi.status() !=  WL_CONNECTED){
        EEPROM.writeString(0,"");
        delay(50);
        EEPROM.writeString(30,"");
        delay(50);
        EEPROM.writeBool(60,1);
        delay(50);
        EEPROM.commit();
        delay(500);
         
        ESP.restart();
    }

  }
    Serial.print("Local IP: ");
   Serial.println(WiFi.localIP());

     display.clearDisplay();
       display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Local IP:");
  display.print(WiFi.localIP());
  display.display();
}

  
  // URL для корневой страницы веб-сервера:
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // URL для файла «style.css»:
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  server.on("/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/bootstrap.min.css", "text/css");
  });

    server.on("/bootstrap.bundle.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/bootstrap.bundle.min.js","text/javascript");
  });

    server.on("/jquery-3.5.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/jquery-3.5.1.min.js","text/javascript");
  });

  // URL для переключения GPIO-контакта на «HIGH»:
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, HIGH);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // URL для переключения GPIO-контакта на «LOW»:
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, LOW);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });


   server.on("/passwordForm", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/passwordForm.html", String(), false, stateNet);
  });

     server.on("/passAndSSID", HTTP_POST, [](AsyncWebServerRequest *request){
       AsyncWebParameter *pass = request->getParam("Password",true);
       AsyncWebParameter *SSID = request->getParam("SSID",true);
       AsyncWebParameter *state = request->getParam("network",true);

        EEPROM.writeString(0,pass->value().c_str());
        delay(50);
        EEPROM.writeString(30,SSID->value().c_str());
        delay(50);
        EEPROM.writeBool(60,state->value().toInt());
        delay(50);
        EEPROM.commit();
        delay(50);

          p = EEPROM.readString(0);
          s = EEPROM.readString(30);
          st = EEPROM.readBool(60);

         Serial.print("--" + s);
         Serial.print("--" + p);
         Serial.print("--" + st);

     request->send(SPIFFS, "/passwordForm.html", String(), false,stateNet);
  
     delay(50);
     ESP.restart();
  });
  // Запускаем сервер:
  server.begin();

}
 
void loop(){
   if(st){
       delay(50000);
       Serial.println("режим AP");
   }
    if (!st){
    delay(50000);
       Serial.println("режим STA");
   }
}
