#define BLYNK_PRINT Serial 
#include "RTClib.h"
//#include <Wire.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <BlynkSimpleEsp8266.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#define REEDINTERRUPT 2
#define tip 0.3 //value of each tip
const char* ssid     = "";
const char* password = "";
int minuta;
int ura;
int sekunda;
volatile int pulseCount_ISR;
const long utcOffsetInSeconds = 3600;
unsigned long lastSecond,last10Minutes;
long lastPulseCount;
int currentPulseCount;


// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
RTC_Millis rtc;     

ICACHE_RAM_ATTR void reedSwitch_ISR()
{
  static unsigned long lastReedSwitchTime;
  // debounce for a quarter second = max. 4 counts per second
  if (labs(millis()-lastReedSwitchTime)>350)
  {
    pulseCount_ISR++;
    Blynk.notify("Palo 0.3 l");
    lastReedSwitchTime=millis();
  }
}
void sendSensor()
{
  float h = currentPulseCount * tip;
  float t = lastPulseCount * tip; 

  String ip;

   ip= WiFi.localIP().toString();  

  
  // You can send any value at any time.

  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V4, h);  //V5 is for Humidity
  Blynk.virtualWrite(V8, t);  //V6 is for Temperature
    Blynk.virtualWrite(V9, WiFi.RSSI()); 
    Blynk.virtualWrite(V10, ip); 
     Blynk.virtualWrite(V12, ura); 
      Blynk.virtualWrite(V13, minuta); 
     
   
  //svitlo da ili ne
}
char auth[] = "7fLc9LIvQjVFra6O-1zgNcWFk8imktAz";
BlynkTimer  timer;

BLYNK_CONNECTED() {
  // Request the latest state from the server
  Blynk.syncVirtual(V1);}
  BLYNK_WRITE(V1) {//reset botun
  
 
   if((param.asInt()==1))
  {
//    ESP.reset();
  ESP.restart();
  }

 }

void setup() {
  Serial.begin(115200);
  
  pinMode(REEDINTERRUPT,INPUT_PULLUP);
  attachInterrupt(REEDINTERRUPT,reedSwitch_ISR, FALLING);
  WiFi.mode(WIFI_STA);
   WiFi.begin(ssid, password); 
   while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  timeClient.begin();
  while(!timeClient.update()) {
  timeClient.forceUpdate();
  Serial.println( "force update" );
}
  rtc.begin(DateTime(timeClient.getEpochTime()));//rtc povuče vrime od ntp 

  Blynk.config(auth);
  Blynk.connect();
 timer.setInterval(4000L, sendSensor);

 // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

   ArduinoOTA.setHostname("Nova mjerač padalina bez senzora");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  
}



void loop()
{
  ArduinoOTA.handle();
  DateTime now = rtc.now();
  ura=now.hour();
  minuta=now.minute();
 /* minuta=timeClient.getMinutes();
  ura=timeClient.getHours();
  sekunda=timeClient.getSeconds()*/
  
  // each second read and reset pulseCount_ISR
  if (millis()-lastSecond>=1000)
  {
    lastSecond+=1000;
    noInterrupts();
    currentPulseCount+=pulseCount_ISR; // add to current counter
    pulseCount_ISR=0; // reset ISR counter
    interrupts();
   // Serial.print(lastPulseCount * tip);Serial.print('\t');Serial.print(currentPulseCount * tip);
   // Serial.println();
  }
   if((ura== 0)&&(minuta==0)) {
   currentPulseCount = 0.0;                                      // clear daily-rain at midnight
                          
  }  
  if((ura== 23)&&(minuta==59)) {
   
   lastPulseCount= currentPulseCount;
                                      
  }  
   Blynk.run(); // Initiates Blynk
  timer.run(); // Initiates SimpleTimer

  // each 10 minutes save data to another counter
  /*if (millis()-last10Minutes>=TENMINUTES)
  {
    last10Minutes+=TENMINUTES; // remember the time
    lastPulseCount+=currentPulseCount; // add to last period Counter
    currentPulseCount=0;; // reset counter for current period
  }*/
}
