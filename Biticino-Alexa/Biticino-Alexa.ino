#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "fauxmoESP.h"
#include "Interfaccia.h"
#include "Ticker.h"



#define WIFI_SSID ""
#define WIFI_PASS ""
Interfaccia interfaccia(D7,D6);                         //Rx, Tx comunicazione


Switch switch_Sala(&interfaccia);                       // comando interfaccia singolo 
GruppoSwitch Generale_Luce(&interfaccia);               // comando interfaccia di gruppo
Serranda serranda_Studio(&interfaccia);         		// comando interfaccia di serranda

fauxmoESP fauxmo;





// -----------------------------------------------------------------------------

#define PAROLA_SWITCH_STUDIO           "studio"
#define PAROLA_GRUPPOLUCE              "gruppo luce"

// -----------------------------------------------------------------------------

void callback_STOP_SERRANDA(void)
{
  serranda_Studio.Stop();  
}

void set_timer_ms(int milli_second) {
  Ticker tick;
  tick.once_ms(milli_second, &callback_STOP_SERRANDA);
}
void set_timer_secondi(int secondi) {
  Ticker tick;
  tick.once(secondi, &callback_STOP_SERRANDA);
}



// -----------------------------------------------------------------------------
// Wifi
// -----------------------------------------------------------------------------

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

  switch_Sala.address(6,1);                            //comando sala indirizzo(AMB6-PL1)
  Generale_Luce.address(0xB1);                         //comando sala indirizzo(GEN LUCE 0XB1)
  serranda_Studio.address(3,6);


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
    fauxmo.addDevice(PAROLA_SWITCH_STUDIO);
    fauxmo.addDevice(PAROLA_GRUPPOLUCE);
    

    fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
        
        // Callback when a command from Alexa is received. 
        // You can use device_id or device_name to choose the element to perform an action onto (relay, LED,...)
        // State is a boolean (ON/OFF) and value a number from 0 to 255 (if you say "set kitchen light to 50%" you will receive a 128 here).
        // Just remember not to delay too much here, this is a callback, exit as soon as possible.
        // If you have to do something more involved here set a flag and process it in your main loop.
        
        Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);

        // Checking for device_id is simpler if you are certain about the order they are loaded and it does not change.
        // Otherwise comparing the device_name is safer.

        if (strcmp(device_name, PAROLA_SWITCH_STUDIO)==0) {

          if(state){
            switch_Sala.On();
          }else{
            switch_Sala.Off();
          }
        } 
/*
  // Converti il numero 0, 255 (100%)  ----> lo converti in 1 secondo(0%) a 10 secondi(100%)
int val = map(val, 0, 255, 1, 10);  
set_timer_secondi(val);

*/



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
