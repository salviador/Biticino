#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ESP8266mDNS.h>
// https://github.com/esp8266/arduino-esp8266fs-plugin
#include <FS.h>
#include <ArduinoJson.h>
#include "periferiche.h"
#include "Interfaccia.h"                                   //LIBRERIA BTICINO INTERFACCIA


// ------------------------------------------------------------------------------------------------


//****************************************
//****************************************
//***************ATTENTIONE***************
//****************************************
//****************************************

//----> Per SCHEDA shield GIALLA <----
//Interfaccia interfaccia(D6,D7);     

//----> Per SCHEDA shield VERDE  <----
Interfaccia interfaccia(D7,D6);     

//****************************************
//****************************************
//****************************************
//****************************************
                                                           
Switch switch_Cucina(&interfaccia);               // comando interfaccia singolo N1
Switch switch_Sala(&interfaccia);                 // comando interfaccia singolo N1
Switch switch_Studio(&interfaccia);               // comando interfaccia singolo N7
Switch switch_Camera(&interfaccia);



#define USE_SERIAL Serial

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);
WebSocketsServer webSocket = WebSocketsServer(81);

/* hostname for mDNS. Should work at least on windows. Try http://esp8266.local */
const char *myHostname = "esp8266";

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
   
    switch(type) {
        case WStype_DISCONNECTED:
        break;
        case WStype_CONNECTED: 
            {
            }
            break;
        
        case WStype_TEXT:
            {
              String _payload = String((char *) &payload[0]);
              StaticJsonDocument<200> jsonBuffer;
              DeserializationError error = deserializeJson(jsonBuffer, _payload);
              if (error) {
                Serial.print("deserializeJson() failed: ");
                Serial.println(error.c_str());
                break;
              }
              JsonObject root = jsonBuffer.as<JsonObject>();
              // Fetch values.
              const char* command = root["command"];

              const char* rele1 = root["r1"];
              const char* rele2 = root["r2"];
              const char* rele3 = root["r3"];
              const char* rele4 = root["r4"];
              const char* rele5 = root["r5"];
              const char* rele6 = root["r6"];
              const char* rele7 = root["r7"];
              const char* rele8 = root["r8"];

              const char* releT = root["rT"];
              
              if(command != NULL){
                int comm1 = (int)root["command"];
                if(comm1 == 10){
                  String RStatus = Get_All_Status_rele_JSON();
                  webSocket.sendTXT(num, RStatus);
                }
              }
              if(rele1 != NULL){
                Set_Status_Relay(1,(uint8_t)root["r1"]);
              }
              if(rele2 != NULL){
                Set_Status_Relay(2,(uint8_t)root["r2"]);
              }
              if(rele3 != NULL){
                Set_Status_Relay(3,(uint8_t)root["r3"]);
              }
              if(rele4 != NULL){
                Set_Status_Relay(4,(uint8_t)root["r4"]);
              }
              if(rele5 != NULL){
                Set_Status_Relay(5,(uint8_t)root["r5"]);
              }
              if(rele6 != NULL){
                Set_Status_Relay(6,(uint8_t)root["r6"]);
              }
              if(rele7 != NULL){
                Set_Status_Relay(7,(uint8_t)root["r7"]);
              }
              if(rele8 != NULL){
                Set_Status_Relay(8,(uint8_t)root["r8"]);
              }
              if(releT != NULL){
              }
                            
              //webSocket.sendTXT(num, "Hurray", strlen("hurray"));
              
            }   
            break;     
             
        case WStype_BIN:
            {
              hexdump(payload, length);
            }
            // echo data back to browser
            webSocket.sendBIN(num, payload, length);
            break;
  
  }
}

void setup() {
  USE_SERIAL.begin(115200);

  interfaccia.begin();

  switch_Cucina.address(2,1);                         //indirizzo(AMB1-PL1)
  switch_Sala.address(2,2);                           //indirizzo(AMB1-PL1)
  switch_Studio.address(2,3);                         //indirizzo(AMB1-PL1)
  switch_Camera.address(2,4);                         //indirizzo(AMB1-PL1)

  
  //allows serving of files from SPIFFS
  SPIFFS.begin();
 
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("Wifi Bticino");

  // modify TTL associated  with the domain name (in seconds)
  // default is 60 seconds
  dnsServer.setTTL(300);
  // set which return code will be used for all other domains (e.g. sending
  // ServerFailure instead of NonExistentDomain will reduce number of queries
  // sent by clients)
  // default is DNSReplyCode::NonExistentDomain
  dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);

  // start DNS server for a specific domain name
  //dnsServer.start(DNS_PORT, "www.example.com", apIP);
  dnsServer.start(DNS_PORT, "*", apIP);

  // simple HTTP server to see that DNS server is working
  webServer.onNotFound([]() {
    webServer.sendHeader("Location", String("http://esp8266.local/index.html"), true);
    webServer.send ( 302, "text/plain", "");
  });

  webServer.serveStatic("/index.html", SPIFFS, "/index.html");
  webServer.serveStatic("/jquery-3.3.1.min.js", SPIFFS, "/jquery-3.3.1.min.js");

  webServer.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
 
  if (!MDNS.begin(myHostname)) {
        Serial.println("Error setting up MDNS responder!");
  } else {
        Serial.println("mDNS responder started");
        // Add service to MDNS-SD
        MDNS.addService("http", "tcp", 80);
  }
}

void loop() {
  dnsServer.processNextRequest();
  webSocket.loop();
  webServer.handleClient();


  switch_update();


  interfaccia.Loop_Seriale();                         //Implementata INTERFACCIA 
}
