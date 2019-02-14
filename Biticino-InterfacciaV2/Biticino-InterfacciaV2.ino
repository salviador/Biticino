#include "Interfaccia.h"

Interfaccia interfaccia(10,11);              //Rx, Tx comunicazione pin Arduino(10-11)


//Dichiarare gli Switch da usare
Switch switch_Stanza_x1(&interfaccia);       // comando interfaccia singolo (Stanza x1) 
Switch switch_Stanza_x2(&interfaccia);       // comando interfaccia singolo (Stanza x2) 
/*Switch switch_Studio_1(&interfaccia);        // comando interfaccia singolo (Studio1)
Switch switch_Studio_2(&interfaccia);        // comando interfaccia singolo (Studio2)
GruppoSwitch Generale_Luce(&interfaccia);    // comando interfaccia di gruppo

Serranda serrandaSala(&interfaccia);         // comando interfaccia di gruppo
*/

const int PULSANTE1 = 6;                     // pin 6  di Arduino
const int PULSANTE2 = 7;                     // pin 7  di Arduino
const int PULSANTE3 = 8;                     // pin 8  di Arduino
const int PULSANTE4 = 9;                     // pin 9  di Arduino
const int PULSANTE5 = 12;                    // pin 12 di Arduino
const int PULSANTE6 = 13;                    // pin 13 di Arduino 


void setup() {
  Serial.begin(115200);
  interfaccia.begin();

  switch_Stanza_x1.address(5,1);             //comando sala indirizzo(AMB6-PL1) 
  switch_Stanza_x2.address(5,2);             //comando sala indirizzo(AMB6-PL2) 
/*  switch_Studio_1.address(3,1);              //comando sala indirizzo(AMB3-PL1)
  switch_Studio_2.address(3,2);              //comando sala indirizzo(AMB3-PL2)
  Generale_Luce.address(0xB1);               //comando sala indirizzo(GEN LUCE 0XB1)
 
  serrandaSala.address(5,5);
*/

  pinMode(PULSANTE1, INPUT_PULLUP);          // ingresso pulsante(1)collegato ad arduino
  pinMode(PULSANTE2, INPUT_PULLUP);          // ingresso pulsante(2)collegato ad arduino
  pinMode(PULSANTE3, INPUT_PULLUP);          // ingresso pulsante(3)collegato ad arduino
  pinMode(PULSANTE4, INPUT_PULLUP);          // ingresso pulsante(4)collegato ad arduino
  pinMode(PULSANTE5, INPUT_PULLUP);          // ingresso pulsante(5)collegato ad arduino
  pinMode(PULSANTE6, INPUT_PULLUP);          // ingresso pulsante(6)collegato ad arduino

}

void loop() {


 //Pulsante 1 , Premuto
  if(digitalRead(PULSANTE1)==0){
   
      switch_Stanza_x1.Toggle();             // Toggle= comando ON/OFF ciclico con un solo pulsante
                                             // Definire la funzione di chi si vuole richiamare(Stanza_x1)
    delay(50);
    delay(50);
    while(digitalRead(PULSANTE1)==0);
    delay(50);
 }
  
 //Pulsante 2 , Premuto
  if(digitalRead(PULSANTE2)==0){
   
      switch_Stanza_x2.Toggle();            // Toggle= comando ON/OFF ciclico con un solo pulsante
                                            // Definire la funzione di chi si vuole richiamare(Stanza_x2)
    delay(50);
    while(digitalRead(PULSANTE2)==0);
    delay(50);
 } 
  /*
 //Pulsante 3 , Premuto
  if(digitalRead(PULSANTE3)==0){
   
      switch_Studio_1.Toggle();             // Toggle= comando ON/OFF ciclico con un solo pulsante
                                            // Definire la funzione di chi si vuole richiamare(Studio_1)
    delay(50);
    delay(50);
    while(digitalRead(PULSANTE3)==0);
    delay(50);
 }

 //Pulsante 4 , Premuto
  if(digitalRead(PULSANTE4)==0){
   
      switch_Studio_2.Toggle();             // Toggle= comando ON/OFF ciclico con un solo pulsante
                                            // Definire la funzione di chi si vuole richiamare(Studio_2)
    delay(50); 
    delay(50);
    while(digitalRead(PULSANTE4)==0);
    delay(50);
 }

 //Pulsante 5 , Premuto                     // Pulsante(5) stessa funzione pulsante(4)
  if(digitalRead(PULSANTE5)==0){
   
      switch_Studio_2.Toggle();             // Toggle= comando ON/OFF ciclico con un solo pulsante 
                                            // Definire la funzione di chi si vuole richiamare(Studio_2)
    delay(50);
    while(digitalRead(PULSANTE5)==0);
    delay(50);
 }

 //Pulsante 6 , Premuto
  if(digitalRead(PULSANTE6)==0){
   
    Generale_Luce.Toggle();                 //Toggle= comando ON/OFF ciclico con un solo pulsante
                                            // Definire la funzione di chi si vuole richiamare(Generale_Luce)
    delay(50);
    while(digitalRead(PULSANTE6)==0);
    delay(50);
 }


  //Pulsante 1 , Premuto
  if(digitalRead(PULSANTE1)==0){
  
    serrandaSala.Toggle();
    
    delay(50);
    while(digitalRead(PULSANTE1)==0);
    delay(50);
    
  }
*/


  
  interfaccia.Loop_Seriale();     //Ok Implementata SWITCH,  ----da verificare Serrande!!----
  
}
