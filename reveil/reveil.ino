

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <Bounce2.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "index.h"

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(12, D4, NEO_GRB + NEO_KHZ800);


const char* ssid = "fabMSTIC";
const char* password = "plusdechocolatx2";

ESP8266WebServer server(80);


int heure=0;
int minute=0;
int aheure=0;
int aminute=0;
int state=0;

#define EEPROMAHEURE 0
#define EEPROMAMINUTE 1

long synctime=0;
long minutetime=0;
long startsynctime=0;
long startpatterntime=0;
long patterntime=0;

#define MIDVAL 50
#define MAXVAL 150

#define PINBUZZER D3
#define PINBUTTON D1

Bounce debouncer = Bounce(); 
void looppattern();

void setPix(int id, int r, int v, int b){
  pixels.setPixelColor(id, pixels.Color(r,v,b));
  pixels.show();
}
void set12Pix(int r, int v, int b){
  pixels.setPixelColor(0, pixels.Color(r,v,b));
  pixels.setPixelColor(1, pixels.Color(r,v,b));
  pixels.setPixelColor(2, pixels.Color(r,v,b));
  pixels.setPixelColor(3, pixels.Color(r,v,b));
  pixels.setPixelColor(4, pixels.Color(r,v,b));
  pixels.setPixelColor(5, pixels.Color(r,v,b));
  pixels.setPixelColor(6, pixels.Color(r,v,b));
  pixels.setPixelColor(7, pixels.Color(r,v,b));
  pixels.setPixelColor(8, pixels.Color(r,v,b));
  pixels.setPixelColor(9, pixels.Color(r,v,b));
  pixels.setPixelColor(10, pixels.Color(r,v,b));
  pixels.setPixelColor(11, pixels.Color(r,v,b));
  pixels.show();
}

void syncHeure(){
        HTTPClient http;
        int inheure=0;
        int inminute=0;
        Serial.print("[HTTP] begin...\n");
        http.begin("http://worldclockapi.com/api/json/cet/now"); //HTTP
        Serial.print("[HTTP] GET...\n");
        int httpCode = http.GET();

        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);
            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                char charBuf[300];
                payload.toCharArray(charBuf,300);
                StaticJsonBuffer<300> jsonBuffer;
                JsonObject& root = jsonBuffer.parseObject(charBuf);
                if (!root.success()) {
                  Serial.println("JSON parsing failed!");
                }
                const char* currentDateTime = root["currentDateTime"];
                inheure=currentDateTime[11]-'0';
                inheure=inheure*10+currentDateTime[12]-'0';
                inminute=currentDateTime[14]-'0';
                inminute=inminute*10+currentDateTime[15]-'0';
                
                Serial.print("Heure recup :");
                Serial.print(inheure);
                Serial.print(":");
                Serial.print(inminute);
                Serial.println();
            }
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end();

        if(state==0)
        {
          startsynctime=millis();
          state++;
          minute=inminute;
        }
        else if(state==1)
        {
          if(minute!=inminute)
          {
            minute=inminute;
            heure=inheure;
            state=2;
            synctime=millis();
            minutetime=synctime;
            Serial.print(heure);
            Serial.print(":");
            Serial.print(minute);
            Serial.println();
          }
        }
}


void handleRoot() {
  String s = MAIN_page;
  server.send(200, "text/html", s);
}


void handleSetAlarme() {
  if (server.arg("heure")!= "" && server.arg("minute")!= ""){   
    aheure=server.arg("heure").toInt();
    aminute=server.arg("minute").toInt();
    server.send(200, "text/plain", "OK");
    EEPROM.write(EEPROMAHEURE, aheure);
    EEPROM.write(EEPROMAMINUTE, aminute);
    EEPROM.commit();
  }
  else
  {
    server.send(400, "text/plain", "Wrong/missing parameters");
  }
}


// wifi manager callback function
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}


void setup(void){
  
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif

  pixels.begin();
  pixels.show();
  pinMode(PINBUZZER,OUTPUT);

  pinMode(PINBUTTON,INPUT_PULLUP);

  // After setting up the button, setup the Bounce instance :
  debouncer.attach(PINBUTTON);
  debouncer.interval(5); // interval in ms
  
  Serial.begin(115200);

  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);
  if(!wifiManager.autoConnect("reveiliot")) 
  {
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  } 
  
  /*WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);*/
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/setalarme", handleSetAlarme);

  server.on("/horloge", [](){
    server.send(200, "text/plain", String(heure)+":"+String(minute));
  });
  server.on("/alarme", [](){
    server.send(200, "text/plain", String(aheure)+":"+String(aminute));
  });

//  server.onNotFound(handleNotFound);
  EEPROM.begin(512);
  aheure = EEPROM.read(EEPROMAHEURE);
  aminute = EEPROM.read(EEPROMAMINUTE);
  server.begin();
  Serial.println("HTTP server started");

set12Pix(0,0,0);
delay(10);
set12Pix(0,0,0);
delay(10);
set12Pix(0,0,0);
delay(10);
set12Pix(0,0,0);
delay(10);
  
  
}

void loop(void){

  if(state==0)
  {
    syncHeure();
  } 
  if(state==1 && millis()-startsynctime > 10000)
  {
    startsynctime=millis();
    syncHeure();
  }
  if(state<3)
  {
    if ( debouncer.rose() ) {
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
    }
  }
  if(state>=2)
  {
    
    if(state==2)pinMode(PINBUZZER,HIGH);
    if(millis()-minutetime>=60000)
    {
      minutetime=millis();
      minute++;
      minute%=60;//minute=minute%60;
      if(minute==0)
      {
        heure++;
        heure%=24;
      }
      Serial.println("add minute");
      afficherHeure();
      
      if(state==2 && aheure==heure && aminute==minute)
      {
        state=3;
        startpatterntime=millis();
        patterntime=millis()-startpatterntime;
        Serial.println("state 3");
      }
      
    }
  }
  if(state==3)
  {
    //Serial.println("ALARME");
    if ( debouncer.rose() ) {
      state=4;
      Serial.println("state 4");
    }
    
    patterntime=millis()-startpatterntime;
    looppattern();
    
  }
  if(state==4)
  {
      pinMode(PINBUZZER,HIGH);
      set12Pix(0,0,0);
      set12Pix(0,0,0);
      set12Pix(0,0,0);
      state=2;
      Serial.println("state 2");
  }
  server.handleClient();
  debouncer.update();
}


void afficherHeure()
{
  int a,d,c;
 pixels.clear();
 a= heure%12;
 d=minute/10;
 c=minute%10;
 int r,v,b;

 for(int i = 0;i<12;i++)
 {
  r=(i==d)? 50 : 0;
  v=(i==c)? 50 : 0;
  b=(i==a)? 50 : 0;
  setPix(i,r,v,b);
 }
}

void looppattern()
{
  if(patterntime<15000) //inférieur à 15 seconde
  {
    set12Pix(0,0,MIDVAL);
  }
  
  if (patterntime>15000 && patterntime<19000)
  {
    int step = patterntime % 6000;
    
    if(step < 500)
    {
      setPix(0,50,00,00);
    }
    else if(step>500 && step<1000)
    {
      setPix(1,00,50,00);
    }
    else if(step>1000 && step<1500)
    {
      setPix(2,00,00,50);
    }
    else if(step>1500 && step<2000)
    {
      setPix(3,50,50,00);
    }
        else if(step>2000 && step<2500)
    {
      setPix(4,00,50,50);
    }
        else if(step>2500 && step<3000)
    {
      setPix(5,50,00,50);
    }
        else if(step>3000 && step<3500)
    {
      setPix(6,50,50,50);
    }
        else if(step>3500 && step<4000)
    {
      setPix(7,50,25,50);
    }
        else if(step>4000 && step<4500)
    {
      setPix(8,25,50,50);
    }
        else if(step>4500 && step<5000)
    {
      setPix(9,50,50,25);
    }    else if(step>5000 && step<5500)
    {
      setPix(10,25,25,50);
    }
        else if(step>5500 && step<6000)
    {
      setPix(11,25,50,25);
    }
  }
  if (patterntime>19000 && patterntime<60000)
  {
    int step = patterntime % 2000;
    if(step < 1000)
    {
      set12Pix(MIDVAL,MIDVAL,MIDVAL);
    } else if(step > 1000  )
    {
      set12Pix(0,0,MIDVAL);
    }
  }
  if (patterntime>30000 && patterntime<60000)
  {
    int step = patterntime%5;
    if(step<1){
      pinMode(PINBUZZER,HIGH);
    }else
    {
      pinMode(PINBUZZER,LOW);

    }
  }
  if (patterntime>60000)
  {
    int stepbuzzer = patterntime%5;
    if(stepbuzzer<1){
      pinMode(PINBUZZER,HIGH);
    }else
    {
      pinMode(PINBUZZER,LOW);

    }
    int step = patterntime % 1500;
    if(step < 500)
    {
      set12Pix(MAXVAL,MAXVAL,MAXVAL);
    } else if(step > 500 && step < 1000)
    {
      set12Pix(0,0,MAXVAL);
    } else
    {
      set12Pix(0,MAXVAL,MAXVAL);
      
    }
  }
}
