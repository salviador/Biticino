#include "FS.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include "Interfaccia.h"                                   //LIBRERIA BTICINO INTERFACCIA

void ACCENDI(String deviceId);
void SPEGNI(String deviceId);


Interfaccia interfaccia(D6,D7);                            //D6"ESP"=TX INTERFACCIA SCS "CON INTERFACCIA NUOVA"
                                                           //D7"ESP"=RX INTERFACCIA SCS "CON INTERFACCIA NUOVA"

                                                           //Aggiungere i Dispositivi Interfaccia

// ------------------------------------------------------------------------------------------------

/*  alexa, accendi led
    alexa, spegni led
*/
Switch Led(&interfaccia);                                         // comando luci o switch
Switch Led2(&interfaccia);                 

/*  alexa, alza finestra bagno
    alexa, abbassa finestra bagno
    alexa, apri finestra bagno
    alexa, chiudi finestra bagno
    alexa, alza del 20% la finestra bagno
    alexa, abbassa del 40% finestra bagno
*/
Serranda serranda_Sala(&interfaccia);                             //Serranda o Tapparelle

//  alexa, sblocca il portone
Serratura serratura_portone(&interfaccia);                        //Serrature

//  alexa, qual'è la temperatura della "zone notte"
TemperatureSensor sensore_zona_Notte(&interfaccia);               //Sensori temperatura

/*
    alexa, imposta 12,3 gradi il termostato

    alexa, aumenta di 5 gradi la temperatura del termostato
    alexa, diminuisci di 5 gradi la temperatura del termostato

    alexa, imposta su raffreddamento il termostato  , dire freddo
    alexa, imposta su riscaldamento il termostato
    alexa, imposta termostato su off
*/
Thermostat termostato(&interfaccia);                              //Termostato


// ------------------------------------------------------------------------------------------------

#define ID_LED                        "endpoint-001"                 //endpoint "NUMEROXXX" presi da aws di amazon (DISPOSITIVI)
#define ID_LED2                       "endpoint-002"

#define ID_SERRANDA_SALA              "tapparella-001"

#define ID_SERRATURA_PORTONE          "locked-001"

#define ID_TEMPERATURA_ZONA_NOTTE     "sensore-001"

#define ID_TERMOSTATO                 "thermostat-001"


// ------------------------------------------------------------------------------------------------

//Alexa -> Gestione Comando LUCI E INTERRUTTORI

                                                            //Funzione ACCENDI LUCI , i dispositivi richiesti da Alexa
void ACCENDI(String deviceId) {
  if (deviceId == ID_LED )
  {  
    Serial.println("Accendo Led");
    Led.On();
  }
 else  if (deviceId == ID_LED2  )
  {  
    Serial.println("Accendo Led2");    
    Led2.On();
  } 
}

                                                            //Funzione SPEGNI LUCI, i dispositivi richiesti da Alexa

void SPEGNI(String deviceId) {
   if (deviceId == ID_LED )
  {  
    Serial.println("Spegno Led");    
    Led.Off();
  }
  
  else if (deviceId == ID_LED2  )
  {  
    Serial.println("Spegno Led2");        
    Led2.Off();
  }   
}


//Alexa -> Gestione Comando SERRANDA
                                                            //Funzione SERRANDA, i dispositivi richiesti da Alexa
void ALEXA_SERRANDA(String deviceID, int valore){
  if(deviceID == SERRANDA_STUDIO)
  {  
    if(valore == 100){
      Serial.println("Serranda Alzo Tutto!");        
      serranda_Studio.Alza(100);
    }else if(valore == 0){
      Serial.println("Serranda Abbasso Tutto!");            
      serranda_Studio.Abbassa(0);
    }else if((valore > 0)&(valore < 100)){
      Serial.println("Serranda action.....");                 
      serranda_Studio.action(valore);
    }    
  }
}


//Alexa -> Gestione Comando SERRATURE

void SERRATURA_SBLOCCA(String deviceId){
  if(deviceId== ID_SERRATURA_PORTONE)
  {  
    Serial.println("Sblocco la serratura del portone");
    serratura_portone.Sblocca();
  }  
}




void TERMOSTATO_IMPOSTA_TEMPERATURA(String deviceId, float target_temp, char mode){
  
  if(mode == 't'){
    if(deviceId == ID_TERMOSTATO){
      Serial.print("Imposto la temperatura del Termostato: ");
      Serial.println(target_temp);      
      termostato.set_temperature(target_temp);
    }    
  }else if (mode == 'a'){
    if(deviceId == ID_TERMOSTATO){    
    }    
  }
}

void TERMOSTATO_IMPOSTA_MODO(String deviceId, String t_mode){
    if(deviceId == ID_TERMOSTATO){
      if(t_mode == "O"){
          Serial.println("Imposto il Termostato su: OFF");       
          termostato.set_mode(0);
      }else if(t_mode == "H"){
        //caldo
          Serial.println("Imposto il Termostato su: CALDO");        
          termostato.set_mode(2);        
      }else if(t_mode == "C"){
        //freddo
          Serial.println("Imposto il Termostato su: FREDDO");        
          termostato.set_mode(1);                
      }
    }   
}


// ------------------------------------------------------------------------------------------------
                                                            //PASWORD del WIFI

const char* ssid = "xxxx";                                   // SSID Del WIFI
const char* password = "xxxxxxxxxxx";                        // Password Del WIFI
// ------------------------------------------------------------------------------------------------

const char* AWS_endpoint = "xxxxxxxxxxxx-ats.iot.xx-xxxx-1.amazonaws.com"; //MQTT broker ip Password del server IoT di AWS Amazon




void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  
  setup_wifi();
  delay(1000); 
  setup_certificate();  
  interfaccia.begin();
  
  //ZONA GIORNO
  
  Led.address(1,3);                             //comando      indirizzo(AMB1-PL1) N1     
  Led2.address(1,1);                            //comando      indirizzo(AMB1-PL1) N2 
    
   //SERRANDE
  
  serranda_Sala.address(2,6);                           //comando       indirizzo(AMB2-PL6) N31

  //SERRATURE
  serratura_portone.address(2);

  //TERMOREGOLAZIONE
  sensore_zona_Notte.address(2);                //Indirizzo Sensore temperatura
  sensore_zona_Notte.Request();

  termostato.address(1);  
}



void loop() {
  //Aggiorna gli stati della luce e switch
  
 if(Led.Is_Change_Stato()){                                                   
      Serial.println("STATO CAMBIATO del Led");
      if(Led.Get_Stato() == 0){                                               
        Serial.println("<ON>");
        send_state_to_Alexa_synchronous(ID_LED,"Alexa.PowerController", "ON");                  
      }else{
        Serial.println("<OFF>");      
        send_state_to_Alexa_synchronous(ID_LED,"Alexa.PowerController", "OFF");        
      }
  }
    
  
  if(Led2.Is_Change_Stato()){                                                  
      Serial.println("STATO CAMBIATO");
      if(Led2.Get_Stato() == 0){                                                
        Serial.println("<ON>");
        send_state_to_Alexa_synchronous(ID_LED2,"Alexa.PowerController", "ON");                  
      }else{
        Serial.println("<OFF>");      
        send_state_to_Alexa_synchronous(ID_LED2,"Alexa.PowerController", "OFF");          
      }
  }



  
  //x Aggiornare lo STATO dei TERMOSTATI
  if(termostato.loop()){
    if(termostato._avaiable==1){  
      termostato._avaiable = 0;
      //Aggiorna la Temperatura Attuale
      String sendT = ID_TERMOSTATO;
      sendT = sendT  + "_T";
      send_state_to_Alexa_synchronous(sendT, "Alexa.ThermostatController", String(termostato.temperature));          //Invia Temperatura ad Alexa
    }
    if(termostato._avaiable==2){  
      termostato._avaiable = 0;
      //Aggiorna la Temperatura di Setting
      String sendT = ID_TERMOSTATO;
      sendT = sendT  + "_S";
      send_state_to_Alexa_synchronous(sendT, "Alexa.ThermostatController", String(termostato.temperature_di_Setting));          //Invia Temperatura ad Alexa
    }
    if((termostato._avaiable==3)|(termostato._avaiable==4)|(termostato._avaiable==5)){  
      //Aggiorna la Temperatura di Setting e Modalita
      String sendT = ID_TERMOSTATO;
      sendT = sendT  + "_M";
      send_state_to_Alexa_synchronous(sendT, "Alexa.ThermostatController", String(termostato._avaiable));          //Invia Temperatura ad Alexa
      String sendTx = ID_TERMOSTATO;
      sendTx = sendTx  + "_S";
      send_state_to_Alexa_synchronous(sendTx, "Alexa.ThermostatController", String(termostato.temperature_di_Setting));          //Invia Temperatura ad Alexa
      
      termostato._avaiable = 0;
    }
  }

  //x Aggiornare lo STATO sei sensori temperatura  
  if(sensore_zona_Notte.available()){
    float t = sensore_zona_Notte.Get();    
    send_state_to_Alexa_synchronous(ID_TEMPERATURA_ZONA_NOTTE, "Alexa.TemperatureSensor", String(t));          //Invia Temperatura ad Alexa
  }



  //x Aggiornare lo STATO delle SERRANDE
  serranda_Sala.timer();





















  //---------------------------------------------------------------------------------
  client_mqtt_loop("ESP8266/out");
  interfaccia.Loop_Seriale();                         //Implementata INTERFACCIA   
}
