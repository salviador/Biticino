
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "fauxmoESP.h"
#include "Interfaccia.h"


#define WIFI_SSID "SSID"
#define WIFI_PASS "password"




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


fauxmoESP fauxmo;

// -----------------------------------------------------------------------------

#define PAROLA_SWITCH_CUCINA                     "Luce_Cucina"            //PAROLA PER ALEXA
#define PAROLA_SWITCH_SALA                       "Luce_Sala"              //PAROLA PER ALEXA
#define PAROLA_SWITCH_STUDIO                     "Luce_Studio"            //PAROLA PER ALEXA
#define PAROLA_SWITCH_CAMERA                     "Luce_Camera"           //PAROLA PER ALEXA

// --------------------------------------S---------------------------------------

// Wifi

void wifiSetup() {

    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);

    // Connect
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Wait
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

  // Connected!
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

}

void setup() {
  Serial.begin(115200);
  interfaccia.begin();

  switch_Cucina.address(2,1);                          //comando nome indirizzo(AMB1-PL1) N1
  switch_Sala.address(2,2);                            //comando  "   indirizzo(AMB1-PL3) N2
  switch_Studio.address(2,3);                          //comando  "   indirizzo(AMB2-PL1) N3
  switch_Camera.address(2,4);                          //comando  "   indirizzo(AMB2-PL2) N4

 // Wifi
    wifiSetup();

    // By default, fauxmoESP creates it's own webserver on the defined port
    // The TCP port must be 80 for gen3 devices (default is 1901)
    // This has to be done before the call to enable()
    fauxmo.createServer(true); // not needed, this is the default value
    fauxmo.setPort(80); // This is required for gen3 devices

    // You have to call enable(true) once you have a WiFi connection
    // You can enable or disable the library at any moment
    // Disabling it will prevent the devices from being discovered and switched
    fauxmo.enable(true);

    // You can use different ways to invoke alexa to modify the devices state:
    // "Alexa, turn yellow lamp on"
    // "Alexa, turn on yellow lamp
    // "Alexa, set yellow lamp to fifty" (50 means 50% of brightness, note, this example does not use this functionality)

    // Add virtual devices
    
    fauxmo.addDevice(PAROLA_SWITCH_CUCINA);                   //DISPOSITIVO ESP PER ALEXA
    fauxmo.addDevice(PAROLA_SWITCH_SALA);                     //DISPOSITIVO ESP PER ALEXA
    fauxmo.addDevice(PAROLA_SWITCH_STUDIO);                   //DISPOSITIVO ESP PER ALEXA
    fauxmo.addDevice(PAROLA_SWITCH_CAMERA);                   //DISPOSITIVO ESP PER ALEXA

 fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
        
        // Callback when a command from Alexa is received. 
        // You can use device_id or device_name to choose the element to perform an action onto (relay, LED,...)
        // State is a boolean (ON/OFF) and value a number from 0 to 255 (if you say "set kitchen light to 50%" you will receive a 128 here).
        // Just remember not to delay too much here, this is a callback, exit as soon as possible.
        // If you have to do something more involved here set a flag and process it in your main loop.
        
        Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);

        // Checking for device_id is simpler if you are certain about the order they are loaded and it does not change.
        // Otherwise comparing the device_name is safer.

   if (strcmp(device_name, PAROLA_SWITCH_CUCINA)==0) {
          if(state){
            switch_Cucina.On();
          }else{
            switch_Cucina.Off();
          }
        } 
        
        if (strcmp(device_name, PAROLA_SWITCH_SALA)==0) {
          if(state){
            switch_Sala.On();
          }else{
            switch_Sala.Off();
          }
        } 

        if (strcmp(device_name, PAROLA_SWITCH_STUDIO)==0) {
          if(state){
            switch_Studio.On();
          }else{
            switch_Studio.Off();
          }
        } 

        if (strcmp(device_name, PAROLA_SWITCH_CAMERA)==0) {
          if(state){
            switch_Camera.On();
          }else{
            switch_Camera.Off();
          }
        } 



    });

}

void loop() {

    // fauxmoESP uses an async TCP server but a sync UDP server
    // Therefore, we have to manually poll for UDP packets
    fauxmo.handle();

    // This is a sample code to output free heap every 5 seconds
    // This is a cheap way to detect memory leaks
    static unsigned long last = millis();
    if (millis() - last > 5000) {
        last = millis();
        Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
    }

    // If your device state is changed by any other means (MQTT, physical button,...)
    // you can instruct the library to report the new state to Alexa on next request:
    // fauxmo.setState(ID_PL1, true, 255);


  

    

  
  interfaccia.Loop_Seriale();     //Ok Implementata SWITCH,  ----da verificare Serrande!!----
}
