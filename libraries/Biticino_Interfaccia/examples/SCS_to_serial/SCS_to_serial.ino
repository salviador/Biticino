  #include <Arduino.h>
#include "espSoftwareSerial.h"


#define LEN_TRAMA_SCS_BUS 7

EspSoftwareSerial biticino = EspSoftwareSerial(D6,D7);




uint8_t val;
uint8_t res;
int8_t risultato = 0;
volatile uint8_t BYTE_TRAMA[LEN_TRAMA_SCS_BUS + 5];
volatile uint8_t STATE_MACHINE_Read_TRAMA;


void setup() {
  Serial.begin(115200);
  biticino.begin(9600);
}

void loop() {

  while(biticino.available()>0){
    val = biticino.read();

    switch(STATE_MACHINE_Read_TRAMA){
      case 0: 
        //FIND A8
        if(val == 0xA8){
          BYTE_TRAMA[0] = val;
          STATE_MACHINE_Read_TRAMA = 1;

        }else{
            //Serial.print(val,HEX);         
            //Serial.print(" ");         
        }
      break;

      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
        //ACQUISISCI 5 byte
          BYTE_TRAMA[STATE_MACHINE_Read_TRAMA] = val;
          STATE_MACHINE_Read_TRAMA++;
      break;
      case 6:
        BYTE_TRAMA[STATE_MACHINE_Read_TRAMA] = val;
      
        if((BYTE_TRAMA[3]==0xFF)&&(BYTE_TRAMA[4]==0x0F)){
          //TRAMA ESTESA
          STATE_MACHINE_Read_TRAMA = 7;
          break;
        }else{
          //FIND A3
          if(val == 0xA3){
              //Verifica se corretto
              res = BYTE_TRAMA[1] ^ BYTE_TRAMA[2];
              res = res ^ BYTE_TRAMA[3];
              res = res ^ BYTE_TRAMA[4];
              if(res == BYTE_TRAMA[5]){
                //Corretto !!!
                risultato = 1;
                //CORRETTO
  //              Serial.println();          
                Serial.println("-----");          
                Serial.print(BYTE_TRAMA[0], HEX);         
                Serial.print(" ");         
                Serial.print(BYTE_TRAMA[1], HEX);         
                Serial.print(" ");         
                Serial.print(BYTE_TRAMA[2], HEX);         
                Serial.print(" ");         
                Serial.print(BYTE_TRAMA[3], HEX);         
                Serial.print(" ");         
                Serial.print(BYTE_TRAMA[4], HEX);         
                Serial.print(" ");         
                Serial.print(BYTE_TRAMA[5], HEX);         
                Serial.print(" ");         
                Serial.println(BYTE_TRAMA[6], HEX);         
                Serial.println("-----");          
                Serial.println("");
            
            }else{
                // ERRORE CHECKSUM
                Serial.print("Errore Frame :");                    
                Serial.print(BYTE_TRAMA[0], HEX);         
                Serial.print(" ");         
                Serial.print(BYTE_TRAMA[1], HEX);         
                Serial.print(" ");         
                Serial.print(BYTE_TRAMA[2], HEX);         
                Serial.print(" ");         
                Serial.print(BYTE_TRAMA[3], HEX);         
                Serial.print(" ");         
                Serial.print(BYTE_TRAMA[4], HEX);         
                Serial.print(" <<[");          
                Serial.print(BYTE_TRAMA[5], HEX);         
                Serial.print("]>> ");                     
                Serial.println(BYTE_TRAMA[6], HEX);         
           }          
        }else{
          Serial.print("Frame Non riconosciuto: ");                    
          Serial.print(BYTE_TRAMA[0], HEX);         
          Serial.print(" ");         
          Serial.print(BYTE_TRAMA[1], HEX);         
          Serial.print(" ");         
          Serial.print(BYTE_TRAMA[2], HEX);         
          Serial.print(" ");         
          Serial.print(BYTE_TRAMA[3], HEX);         
          Serial.print(" ");         
          Serial.print(BYTE_TRAMA[4], HEX);         
          Serial.print(" ");         
          Serial.print(BYTE_TRAMA[5], HEX);         
          Serial.print(" ");         
          Serial.println(BYTE_TRAMA[6], HEX);         
         }
        STATE_MACHINE_Read_TRAMA = 0;        
      }
      break;

      case 7:
      case 8:
      case 9:
        //ACQUISISCI 5 byte
          BYTE_TRAMA[STATE_MACHINE_Read_TRAMA] = val;
          STATE_MACHINE_Read_TRAMA++;
      break;
      case 10:
        BYTE_TRAMA[STATE_MACHINE_Read_TRAMA] = val;        
        if(val == 0xA3){
          //Verifica se corretto
          //check sum da rivedere
            res = BYTE_TRAMA[1] ^ BYTE_TRAMA[2];
            res = res ^ BYTE_TRAMA[3];
            res = res ^ BYTE_TRAMA[4];
            res = res ^ BYTE_TRAMA[5];
            res = res ^ BYTE_TRAMA[6];
            res = res ^ BYTE_TRAMA[7];
            res = res ^ BYTE_TRAMA[8];
 
           if(res == BYTE_TRAMA[9]){    //?????????????????
              //Corretto !!!
              Serial.println("-----FRAME ESTESO-----");          
              Serial.print(BYTE_TRAMA[0], HEX);         
              Serial.print(" ");         
              Serial.print(BYTE_TRAMA[1], HEX);         
              Serial.print(" ");         
              Serial.print(BYTE_TRAMA[2], HEX);         
              Serial.print(" ");         
              Serial.print(BYTE_TRAMA[3], HEX);         
              Serial.print(" ");         
              Serial.print(BYTE_TRAMA[4], HEX);         
              Serial.print(" ");         
              Serial.print(BYTE_TRAMA[5], HEX);         
              Serial.print(" ");         
              Serial.print(BYTE_TRAMA[6], HEX);         
              Serial.print(" ");         
              Serial.print(BYTE_TRAMA[7], HEX);         
              Serial.print(" ");         
              Serial.print(BYTE_TRAMA[8], HEX);         
              Serial.print(" ");         
              Serial.print(BYTE_TRAMA[9], HEX);         
              Serial.print(" ");         
              Serial.println(BYTE_TRAMA[10], HEX);         
              Serial.println("-----");          
          }else{
              //Errore checksum
              Serial.println("-----Errore FRAME ESTESO-----");          
              Serial.print(BYTE_TRAMA[0], HEX);         
              Serial.print(" ");         
              Serial.print(BYTE_TRAMA[1], HEX);         
              Serial.print(" ");         
              Serial.print(BYTE_TRAMA[2], HEX);         
              Serial.print(" ");         
              Serial.print(BYTE_TRAMA[3], HEX);         
              Serial.print(" ");         
              Serial.print(BYTE_TRAMA[4], HEX);         
              Serial.print(" ");         
              Serial.print(BYTE_TRAMA[5], HEX);         
              Serial.print(" ");         
              Serial.print(BYTE_TRAMA[6], HEX);         
              Serial.print(" ");         
              Serial.print(BYTE_TRAMA[7], HEX);         
              Serial.print(" ");         
              Serial.print(BYTE_TRAMA[8], HEX);         
              Serial.print(" ");         
              Serial.print(BYTE_TRAMA[9], HEX);         
              Serial.print(" ");         
              Serial.println(BYTE_TRAMA[10], HEX);         
              Serial.println("-----");     
           }
        }else{
          Serial.print("Frame Non riconosciuto: ");                             
          Serial.print(BYTE_TRAMA[0], HEX);         
          Serial.print(" ");         
          Serial.print(BYTE_TRAMA[1], HEX);         
          Serial.print(" ");         
          Serial.print(BYTE_TRAMA[2], HEX);         
          Serial.print(" ");         
          Serial.print(BYTE_TRAMA[3], HEX);         
          Serial.print(" ");         
          Serial.print(BYTE_TRAMA[4], HEX);         
          Serial.print(" ");         
          Serial.print(BYTE_TRAMA[5], HEX);         
          Serial.print(" ");         
          Serial.print(BYTE_TRAMA[6], HEX);         
          Serial.print(" ");         
          Serial.print(BYTE_TRAMA[7], HEX);         
          Serial.print(" ");         
          Serial.print(BYTE_TRAMA[8], HEX);         
          Serial.print(" ");         
          Serial.print(BYTE_TRAMA[9], HEX);         
          Serial.print(" ");         
          Serial.println(val, HEX);         
         }
        STATE_MACHINE_Read_TRAMA = 0;
      break;
      
    }
   
  }
  


}
