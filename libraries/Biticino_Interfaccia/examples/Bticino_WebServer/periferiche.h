#include "Interfaccia.h"                                   //LIBRERIA BTICINO INTERFACCIA

extern Interfaccia interfaccia;          
extern Switch switch_Cucina;               // comando interfaccia singolo N1
extern Switch switch_Sala;                 // comando interfaccia singolo N1
extern Switch switch_Studio;               // comando interfaccia singolo N7
extern Switch switch_Camera;


extern WebSocketsServer webSocket;
String Get_All_Status_rele_JSON(void);
void Set_Status_Relay(uint8_t nRelay, uint8_t stato);
void switch_update(void);

//https://techtutorialsx.com/2017/04/02/esp8266-nodemcu-pin-mappings/

struct _rele {
  unsigned char rele1;
  unsigned char rele2;
  unsigned char rele3;
  unsigned char rele4;
  unsigned char rele5;
  unsigned char rele6;
  unsigned char rele7;
  unsigned char rele8;  
}RELE;



String Get_All_Status_rele_JSON(void){
  String s = "{\"r1\":\"" + String(RELE.rele1) + "\"," +
              "\"r2\":\"" + String(RELE.rele2) + "\"," +
              "\"r3\":\"" + String(RELE.rele3) + "\"," +
              "\"r4\":\"" + String(RELE.rele4) + "\"," +
              "\"r5\":\"" + String(RELE.rele5) + "\"," +
              "\"r6\":\"" + String(RELE.rele6) + "\"," +
              "\"r7\":\"" + String(RELE.rele7) + "\"," +
              "\"r8\":\"" + String(RELE.rele8) + "\"}";
  return s;
}


void Set_Status_Relay(uint8_t nRelay, uint8_t stato){
  switch(nRelay){
    case 1:
      if(stato){
        switch_Cucina.On();
        RELE.rele1 = 1;
      }else{
        switch_Cucina.Off();
        RELE.rele1 = 0;        
      }
    break;
    case 2:
      if(stato){
        switch_Sala.On();
        RELE.rele2 = 1;        
      }else{
        switch_Sala.Off();
        RELE.rele2 = 0;        
      }
    break;
    case 3:
      if(stato){
        switch_Studio.On();
        RELE.rele3 = 1;
      }else{
        switch_Studio.Off();
        RELE.rele3 = 0;        
      }
    break;
    case 4:
      if(stato){
        switch_Camera.On();
        RELE.rele4 = 1;        
      }else{
        switch_Camera.Off();
        RELE.rele4 = 0;        
      }
    break;
    case 5:
    break;
    case 6:
    break;
    case 7:
    break;
    case 8:
    break;
    default:
    break;
  }
} 


void switch_update(void){  
 if(switch_Cucina.Is_Change_Stato()){                        
    if(switch_Cucina.Get_Stato() == 0){                      
      String s = "{\"r1\":\"" + String(RELE.rele1) + "\"}";
      webSocket.sendTXT(0,s);
      RELE.rele1 = 1;
    }else{
      String s = "{\"r1\":\"" + String(RELE.rele1) + "\"}";
      webSocket.sendTXT(0,s);
      RELE.rele1 = 0;      
    }
  }

 if(switch_Sala.Is_Change_Stato()){                        
    if(switch_Sala.Get_Stato() == 0){                      
      String s = "{\"r2\":\"" + String(RELE.rele2) + "\"}";
      webSocket.sendTXT(0,s);
      RELE.rele2 = 1;      
    }else{
      String s = "{\"r2\":\"" + String(RELE.rele2) + "\"}";
      webSocket.sendTXT(0,s);
      RELE.rele2 = 0;      
    }
  }

 if(switch_Studio.Is_Change_Stato()){                        
    if(switch_Studio.Get_Stato() == 0){                      
      String s = "{\"r3\":\"" + String(RELE.rele3) + "\"}";
      webSocket.sendTXT(0,s);
      RELE.rele3 = 1;      
    }else{
      String s = "{\"r3\":\"" + String(RELE.rele3) + "\"}";
      webSocket.sendTXT(0,s);
      RELE.rele3 = 0;      
    }
  }


 if(switch_Camera.Is_Change_Stato()){                        
    if(switch_Camera.Get_Stato() == 0){                      
      String s = "{\"r4\":\"" + String(RELE.rele4) + "\"}";
      webSocket.sendTXT(0,s);
      RELE.rele4 = 1;      
    }else{
      String s = "{\"r4\":\"" + String(RELE.rele4) + "\"}";
      webSocket.sendTXT(0,s);
      RELE.rele4 = 0;      
    }
  }

 
}
