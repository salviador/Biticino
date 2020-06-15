#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>                              // https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <ArduinoJson.h>                                   // https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <StreamString.h>
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
Interfaccia interfaccia(D6,D7);     

//****************************************
//****************************************
//****************************************
//****************************************


                                                           //Aggiungere i Dispositivi Interfaccia
                                                           

//ZONA GIORNO LUCI                                           

Switch switch_Cucina(&interfaccia);                 // comando interfaccia singolo N1
Switch switch_Sala(&interfaccia);                 // comando interfaccia singolo N2
Switch switch_Studio(&interfaccia);                // comando interfaccia singolo N3
Switch switch_Camera(&interfaccia);                // comando interfaccia singolo N4

GruppoSwitch Generale_Luce(&interfaccia);                  // comando interfaccia di gruppo

Serranda serranda_Sala(&interfaccia);                      // comando serranda



// ------------------------------------------------------------------------------------------------
 
                                                            //PASWORD di Sinrc e del WIFI

#define MyApiKey "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxx"     // API Key, Copiarla dal sito Sinrc., una volta immessa NON serve più cambiarla
#define MySSID "SSID"                                        // SSID Del WIFI
#define MyWifiPassword "password"                       // Password Del WIFI

// ------------------------------------------------------------------------------------------------
                                                            // Per ogni Dispositivo , c'è una password univoca che il sito SinrIc 
                                                            // dà per ogSni dispositivo creato

//ZONA GIORNO LUCI                                          //Indirizzi da copiare dalla app Sinric               

#define ID_CUCINA "5cac8597b188e06c90865f2a"                  
#define ID_SALA "5cac85b0b188e06c90865f34"
#define ID_STUDIO "5cac85d1b188e06c90865f42"
#define ID_CAMERA "5cac85e2b188e06c90865f4c"
#define ID_GENERALE_LUCE  "5cac8602b188e06c90865f5a"
#define ID_SERRANDA_SALA  "5cac8939b188e06c908660c2"


// ------------------------------------------------------------------------------------------------

                                                            //Funzione ACCENDI LUCI , i dispositivi richiesti da Alexa
void ACCENDI(String deviceId) {                     
  if (deviceId == ID_CUCINA )
  {  
    switch_Cucina.On();
  }
 else  if (deviceId == ID_SALA  )
  {  
    switch_Sala.On();
  }  

 else  if (deviceId == ID_STUDIO )
  {  
    switch_Studio.On();
  }
  
 else  if (deviceId == ID_CAMERA)
  {  
    switch_Camera.On();
  }  

 else  if (deviceId == ID_GENERALE_LUCE)
  {  
    Generale_Luce.On();
  }
}

                                                   //Funzione SPEGNI LUCI, i dispositivi richiesti da Alexa

void SPEGNI(String deviceId) {
   if (deviceId == ID_CUCINA )
  {  
    switch_Cucina.Off();
  }
else  if (deviceId == ID_SALA  )
  {  
    switch_Sala.Off();
  }  

else  if (deviceId == ID_STUDIO )
  {  
    switch_Studio.Off();
  }
  
else  if (deviceId == ID_CAMERA)
  {  
    switch_Camera.Off();
  }  

else  if (deviceId == ID_GENERALE_LUCE)
  {  
    Generale_Luce.Off();
  }
  
 // COMANDI PER SERRANDE
 //---------------------------------------------- COMANDO FERMA ---------------------------------------------------
 
 //Alexa , FERMA  serranda xxxxx  
  if (deviceId == ID_SERRANDA_SALA)
  {  
    Serial.print("STO FERMANDO [STOP] SERRANDA ");   

    serranda_Sala.Stop();
  }
 } 



//---------------------------------------------- COMANDO ALZA ------------------------------------------------------

void ALZA(String deviceId, int percentuale) { 
  
 //Alexa , ALZA  serranda xxxxx 
 if (deviceId == ID_SERRANDA_SALA)
  {  
    Serial.println("STO ALZANDO SERRANDA");

    serranda_Sala.Alza();
  }
}

//---------------------------------------------- COMANDO ABBASSA ---------------------------------------------------

void ABBASSA(String deviceId, int percentuale) {
  
 //Alexa , ABBASSA  serranda xxxxx
 if (deviceId == ID_SERRANDA_SALA)
  {  
    Serial.println("STO ABBASSANDO SERRANA ");   

   serranda_Sala.Abbassa();
  }

}



// ------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  interfaccia.begin();
  
  sinric_connetti();
                                                       
                                                 // assegna gli INDIRIZZI ai dispositivi BTICINO
  //ZONA GIORNO
  
   switch_Cucina.address(1,3);            //comando      indirizzo(AMB1-PL1) N1     
   switch_Sala.address(1,1);            //comando      indirizzo(AMB1-PL3) N2    
   switch_Studio.address(2,1);           //comando      indirizzo(AMB2-PL1) N3       
   switch_Camera.address(2,2);           //comando      indirizzo(AMB2-PL2) N4    
 
   Generale_Luce.address(0xB1);                   //comando      indirizzo(GEN LUCE 0XB1) N25

   //motori serrande
   serranda_Sala.address(1,6);           //comando       indirizzo(AMB1-PL6) N26

}

void loop() {

//CAMBIO STATO FEEDBACK
  
if(switch_Cucina.Is_Change_Stato()){                 //LUCI CUCINA 
    Serial.println("STATO CAMBIATO");
    if(switch_Cucina.Get_Stato() == 0){              //LUCI CUCINA 
      Serial.println("<ON>");
      setPowerStateOnServer(ID_CUCINA, "ON");           //INDIRIZZO DISPOSITIVO ASSEGNATO
    }else{
      Serial.println("<OFF>");      
      setPowerStateOnServer(ID_CUCINA, "OFF");          //INDIRIZZO DISPOSITIVO ASSEGNATO
    }
  }
  

if(switch_Sala.Is_Change_Stato()){                 //LUCI  sala
    Serial.println("STATO CAMBIATO");
    if(switch_Sala.Get_Stato() == 0){              //LUCI  sala
      Serial.println("<ON>");
      setPowerStateOnServer(ID_SALA, "ON");           //INDIRIZZO DISPOSITIVO ASSEGNATO
    }else{
      Serial.println("<OFF>");      
      setPowerStateOnServer(ID_SALA, "OFF");          //INDIRIZZO DISPOSITIVO ASSEGNATO
    }
  }


  if(switch_Studio.Is_Change_Stato()){              //LUCI  studio
    Serial.println("STATO CAMBIATO");
    if(switch_Studio.Get_Stato() == 0){             //LUCI  studio
      Serial.println("<ON>");
      setPowerStateOnServer(ID_STUDIO, "ON");          //INDIRIZZO DISPOSITIVO ASSEGNATO
    }else{
      Serial.println("<OFF>");      
      setPowerStateOnServer(ID_STUDIO, "OFF");         //INDIRIZZO DISPOSITIVO ASSEGNATO
    }
  }


  if(switch_Camera.Is_Change_Stato()){              //LUCI  camera
    Serial.println("STATO CAMBIATO");
    if(switch_Camera.Get_Stato() == 0){             //LUCI  camera
      Serial.println("<ON>");
      setPowerStateOnServer(ID_CAMERA, "ON");          //INDIRIZZO DISPOSITIVO ASSEGNATO
    }else{
      Serial.println("<OFF>");      
      setPowerStateOnServer(ID_CAMERA, "OFF");         //INDIRIZZO DISPOSITIVO ASSEGNATO
    }
  }

 if(Generale_Luce.Is_Change_Stato()){                        //LUCI GENERALE CASA 
    Serial.println("STATO CAMBIATO");
    if(Generale_Luce.Get_Stato() == 0){                      //LUCI GENERALE CASA 
      Serial.println("<ON>");
      setPowerStateOnServer(ID_GENERALE_LUCE , "ON");    //INDIRIZZO DISPOSITIVO ASSEGNATO
    }else{
      Serial.println("<OFF>");      
      setPowerStateOnServer(ID_GENERALE_LUCE , "OFF");   //INDIRIZZO DISPOSITIVO ASSEGNATO
    }
  }








  
  sinric_loop();                                      //Implementata Sinric
  interfaccia.Loop_Seriale();                         //Implementata INTERFACCIA 
}
