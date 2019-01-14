#include "Interfaccia.h"

Interfaccia interfaccia(10,11);     //Rx, Tx


Switch switch_Sala(&interfaccia);
Serranda serrandaSala(&interfaccia);
GruppoSwitch Luci_Giorno(&interfaccia);



const int PULSANTE1 = 6;


void setup() {
  Serial.begin(115200);
  interfaccia.begin();

  switch_Sala.address(6,1);   //ok da provare
  serrandaSala.address(3,2);  
  Luci_Giorno.address(0xB2);  //ok da provare
 
  serrandaSala.address(5,5);


  pinMode(PULSANTE1, INPUT_PULLUP);

}

void loop() {

  //Pulsante 1 , Premuto
  if(digitalRead(PULSANTE1)==0){
  
    serrandaSala.Toggle();
    
    delay(50);
    while(digitalRead(PULSANTE1)==0);
    delay(50);
    
  }

/*
  if(PULSANTE1==1){
    
    switch_Sala.On();
    
  }
  if(PULSANTE1==1){
    
    switch_Sala.Off();
    
  }
  if(PULSANTE1==1){
    
    switch_Sala.Toggle();
    
  }
*/


  
  interfaccia.Loop_Seriale();     //Ok Implementata SWITCH,  ----da verificare Serrande!!----
  
}
