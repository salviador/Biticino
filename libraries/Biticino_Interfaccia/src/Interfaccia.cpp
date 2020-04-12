#include "Interfaccia.h"


uint8_t _pValueDimmer[12] = {0x01, 0x0D,0x1D,0x2D,0x3D,0x4D,0x5D,0x6D,0x7D,0x8D,0x9D};

union
{
    unsigned long val;
    unsigned char buffer[4];
}convertByteArryTo32;



Interfaccia::Interfaccia(int rx, int tx){
	_rx = rx;
	_tx = tx;
	
	#if defined(__AVR__)
		scs = new SoftwareSerial(_rx,_tx);
	#elif defined(ESP8266)
		scs = new EspSoftwareSerial(_rx, _tx, false, 256);
	#endif	
	
	STATE_MACHINE_Read_TRAMA = 0;

  _ctn_interfacee=0;
}
void Interfaccia::begin(void){
	scs->begin(9600);
}
int8_t Interfaccia::search_TRAMA(uint8_t START, uint8_t FINISH){
	uint8_t val;
	uint8_t res;
	int8_t risultato = 0;

	while(scs->available()>0){
		val = scs->read();
		
		//Serial.println(val, HEX);
		switch(STATE_MACHINE_Read_TRAMA){
			case 0:	
				//FIND A8
				if(val == START){
					BYTE_TRAMA[0] = val;
					STATE_MACHINE_Read_TRAMA = 1;
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

          if(BYTE_TRAMA[STATE_MACHINE_Read_TRAMA]!=FINISH){
        //if((BYTE_TRAMA[3]==0xFF)&&(BYTE_TRAMA[4]==0x0F)){
          //TRAMA ESTESA
          STATE_MACHINE_Read_TRAMA = 7;
          break;
        }else{
          //FIND A3
          if(val == FINISH){
              //Verifica se corretto
              res = BYTE_TRAMA[1] ^ BYTE_TRAMA[2];
              res = res ^ BYTE_TRAMA[3];
              res = res ^ BYTE_TRAMA[4];
              if(res == BYTE_TRAMA[5]){
                //Corretto !!!
                risultato = 1;
                //CORRETTO
            }
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
				if(val == FINISH){
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
            risultato = 2;  //TRAMA ESTESA
            //Serial.println("-STATE_MACHINE_Read_TRAMA-  *TRAMA TROVATA - CORRETTO-*");				
          }
				}
				STATE_MACHINE_Read_TRAMA = 0;
      break;


		}
		if(risultato>0){      
      //return risultato;
			break;			
		}
	}
	return risultato;
}

void Interfaccia::Loop_Seriale(){
	int8_t val;
	uint8_t add;
	uint8_t addA;
	uint8_t addPL;
	int8_t result = 0;

	val = search_TRAMA(0xA8, 0xA3);
	if(val == 1) {
		//Trama TROVATA
		if(BYTE_TRAMA[1] == 0xB8){	//Richiesta Comando
	
	
	//Serial.println("vvvvvvvvvvvvvvvv");
	
			// verifica indirizzi Interessati delle varie interfaccie caricate

      for(int i=0; i<_ctn_interfacee; i++){
        TYPE_INTERfACCIA_t t = _interfacee[i]->Get_Type();
        //Leggi contenuto della seriale e aggiorna stato dispositivi interfaccie
        if(t==SWITCH){
          //Vedi se la risposta della seriale � compatibile con questo indirizzo
          
          addA = _interfacee[i]->Get_Address_A();
          addPL = _interfacee[i]->Get_Address_PL();
          //[Codifica add]
          add = 0;
          add = (addA & 0x0F);
          add = add << 4;
          add = add & 0xF0;
          add = add | addPL;

          if(add == BYTE_TRAMA[2]){	//A+PL corisponde
             _interfacee[i]->Set_Stato(BYTE_TRAMA[4]);
			// Serial.println(" _interfacee[i]->Set_Stato(BYTE_TRAMA[4]);");
          }	
        }else if(t==DIMMER){
          //Vedi se la risposta della seriale � compatibile con questo indirizzo
          
          addA = _interfacee[i]->Get_Address_A();
          addPL = _interfacee[i]->Get_Address_PL();
          //[Codifica add]
          add = 0;
          add = (addA & 0x0F);
          add = add << 4;
          add = add & 0xF0;
          add = add | addPL;

          if(add == BYTE_TRAMA[2]){	//A+PL corisponde
             _interfacee[i]->Set_Stato(BYTE_TRAMA[4]);
			// Serial.println(" _interfacee[i]->Set_Stato(BYTE_TRAMA[4]);");
          }	
        }else if(t==SERRANDA){
          //Vedi se la risposta della seriale � compatibile con questo indirizzo
          
          addA = _interfacee[i]->Get_Address_A();
          addPL = _interfacee[i]->Get_Address_PL();
          //[Codifica add]
          add = 0;
          add = (addA & 0x0F);
          add = add << 4;
          add = add & 0xF0;
          add = add | addPL;

          if(add == BYTE_TRAMA[2]){	//A+PL corisponde
            if(BYTE_TRAMA[4] != 0x0A){
                _interfacee[i]->Set_Stato(BYTE_TRAMA[4]);
            }

            if((BYTE_TRAMA[1] == 0xB8)&& (BYTE_TRAMA[3] == 0x12)&&(BYTE_TRAMA[4] == 0x08)){	//Serranda x sincronismo ALZA
              Serial.println("ALZO MANUALE ***");
                  _interfacee[i]->buffer[0] = 2;
                  convertByteArryTo32.val = millis();
                  _interfacee[i]->buffer[1] = convertByteArryTo32.buffer[0];
                  _interfacee[i]->buffer[2] = convertByteArryTo32.buffer[1];
                  _interfacee[i]->buffer[3] = convertByteArryTo32.buffer[2];
                  _interfacee[i]->buffer[4] = convertByteArryTo32.buffer[3];

            }else if((BYTE_TRAMA[1] == 0xB8)&& (BYTE_TRAMA[3] == 0x12)&&(BYTE_TRAMA[4] == 0x09)){	//Serranda x sincronismo ABBASSA
            Serial.println("ABBASSO MANUALE ***");
                _interfacee[i]->buffer[0] = 1;       
                  convertByteArryTo32.val = millis();
                  _interfacee[i]->buffer[1] = convertByteArryTo32.buffer[0];
                  _interfacee[i]->buffer[2] = convertByteArryTo32.buffer[1];
                  _interfacee[i]->buffer[3] = convertByteArryTo32.buffer[2];
                  _interfacee[i]->buffer[4] = convertByteArryTo32.buffer[3];
            }else if((BYTE_TRAMA[1] == 0xB8)&& (BYTE_TRAMA[3] == 0x12)&&(BYTE_TRAMA[4] == 0x0A)){	//Serranda x sincronismo FERMA
              Serial.println("ABBASSO STOP ***");
                  _interfacee[i]->buffer[0] = 3;
                  convertByteArryTo32.val = millis();
                  _interfacee[i]->buffer[1] = convertByteArryTo32.buffer[0];
                  _interfacee[i]->buffer[2] = convertByteArryTo32.buffer[1];
                  _interfacee[i]->buffer[3] = convertByteArryTo32.buffer[2];
                  _interfacee[i]->buffer[4] = convertByteArryTo32.buffer[3];
            }

          }	

        }
      }
		}else if(BYTE_TRAMA[1] == 0xB4){	//sensori Temperature

      for(int i=0; i<_ctn_interfacee; i++){
        TYPE_INTERfACCIA_t t = _interfacee[i]->Get_Type();
        //Leggi contenuto della seriale e aggiorna stato dispositivi interfaccie
        if(t==TEMPERATURE_SENSOR){
          //Vedi se la risposta della seriale � compatibile con questo indirizzo
         
          addA = _interfacee[i]->Get_Address_A();

          if(addA == BYTE_TRAMA[2]){	//Address corisponde

             _interfacee[i]->Set_Stato(1);
            _interfacee[i]->buffer[0]=BYTE_TRAMA[4];              //_interfacee[i]->Set_Temp_RAW(BYTE_TRAMA[4]);
			// Serial.println(" _interfacee[i]->Set_Stato(BYTE_TRAMA[4]);");
          }	
        }       
      }
    }else if((BYTE_TRAMA[1] == 0x91)&& (BYTE_TRAMA[3] == 0x60)&&(BYTE_TRAMA[4] == 0x08)){	//Campanello
      for(int i=0; i<_ctn_interfacee; i++){
        TYPE_INTERfACCIA_t t = _interfacee[i]->Get_Type();
        //Leggi contenuto della seriale e aggiorna stato dispositivi interfaccie
        if(t==CAMPANELLO){
          //Vedi se la risposta della seriale � compatibile con questo indirizzo
         
          addA = _interfacee[i]->Get_Address_A();

          if(addA == BYTE_TRAMA[2]){	//Address corisponde

             _interfacee[i]->Set_Stato(1);
			// Serial.println(" _interfacee[i]->Set_Stato(BYTE_TRAMA[4]);");
          }	
        }       
      }
     }



	}else if(val == 2){
    /*
      Serial.print(BYTE_TRAMA[0],HEX);
      Serial.print(" ");
      Serial.print(BYTE_TRAMA[1],HEX);
      Serial.print(" ");
      Serial.print(BYTE_TRAMA[2],HEX);
      Serial.print(" ");
      Serial.print(BYTE_TRAMA[3],HEX);
      Serial.print(" ");
      Serial.print(BYTE_TRAMA[4],HEX);
      Serial.print(" ");
      Serial.print(BYTE_TRAMA[5],HEX);
      Serial.print(" ");
      Serial.print(BYTE_TRAMA[6],HEX);
      Serial.print(" ");
      Serial.print(BYTE_TRAMA[7],HEX);
      Serial.print(" ");
      Serial.print(BYTE_TRAMA[8],HEX);
      Serial.print(" ");
      Serial.print(BYTE_TRAMA[9],HEX);
      Serial.print(" ");
      Serial.println(BYTE_TRAMA[10],HEX);
    */

    //Trama ESTESA
		if(BYTE_TRAMA[1] == 0xEC){	//Richiesta Comando
	
			// verifica indirizzi Interessati delle varie interfaccie caricate

      for(int i=0; i<_ctn_interfacee; i++){
        TYPE_INTERfACCIA_t t = _interfacee[i]->Get_Type();
        //Leggi contenuto della seriale e aggiorna stato dispositivi interfaccie
        if(t==GRUPPO){
          //Vedi se la risposta della seriale � compatibile con questo indirizzo
          
          addA = _interfacee[i]->Get_Address_A();
          addPL = _interfacee[i]->Get_Address_PL();
          //[Codifica add]
          add = 0;
          add = (addA & 0x0F);
          add = add << 4;
          add = add & 0xF0;
          add = add | addPL;

          if(add == BYTE_TRAMA[5]){	//A+PL corisponde
             _interfacee[i]->Set_Stato(BYTE_TRAMA[8]);
          }	
        }
      }

    }else if((BYTE_TRAMA[1] == 0xD2) && (BYTE_TRAMA[3] == 0x03) && (BYTE_TRAMA[4] == 0x04)&&
      (BYTE_TRAMA[5] == 0xC0)){
      //TERMOSTATO <<temperatura >>
      for(int i=0; i<_ctn_interfacee; i++){
        TYPE_INTERfACCIA_t t = _interfacee[i]->Get_Type();
        //Leggi contenuto della seriale e aggiorna stato dispositivi interfaccie
        if(t==THERMOSTAT){
          //Vedi se la risposta della seriale � compatibile con questo indirizzo
         
          addA = _interfacee[i]->Get_Address_A();

          if(addA == BYTE_TRAMA[2]){	//Address corisponde
            if(BYTE_TRAMA[7]!=0x00){
              _interfacee[i]->Set_Stato(1);
              _interfacee[i]->buffer[0]=BYTE_TRAMA[7];           
            }
          }	
        }       
      }
    }else if((BYTE_TRAMA[1] == 0xD2) && (BYTE_TRAMA[3] == 0x03) && (BYTE_TRAMA[4] == 0x04)&&
      (BYTE_TRAMA[5] == 0x12)){
      //TERMOSTATO <<temperature di Setting>> e <<Modalita freddo o caldo>>
      for(int i=0; i<_ctn_interfacee; i++){
        TYPE_INTERfACCIA_t t = _interfacee[i]->Get_Type();
        //Leggi contenuto della seriale e aggiorna stato dispositivi interfaccie
        if(t==THERMOSTAT){
          //Vedi se la risposta della seriale � compatibile con questo indirizzo
         
          addA = _interfacee[i]->Get_Address_A();

          if(addA == BYTE_TRAMA[2]){	//Address corisponde

            _interfacee[i]->Set_Stato(2);
            _interfacee[i]->buffer[1]=BYTE_TRAMA[6];           

            _interfacee[i]->buffer[3]=BYTE_TRAMA[7];            //Qui temp
            _interfacee[i]->buffer[4]=BYTE_TRAMA[8];           

          }	
        }       
      }
    }else if((BYTE_TRAMA[1] == 0xD2) && (BYTE_TRAMA[3] == 0x03) && (BYTE_TRAMA[4] == 0x04)&&
      (BYTE_TRAMA[5] == 0x0E)){
      //TERMOSTATO <<temperature di Setting>>
      for(int i=0; i<_ctn_interfacee; i++){
        TYPE_INTERfACCIA_t t = _interfacee[i]->Get_Type();
        //Leggi contenuto della seriale e aggiorna stato dispositivi interfaccie
        if(t==THERMOSTAT){
          //Vedi se la risposta della seriale � compatibile con questo indirizzo
         
          addA = _interfacee[i]->Get_Address_A();

          if(addA == BYTE_TRAMA[2]){	//Address corisponde

            _interfacee[i]->Set_Stato(3);
            _interfacee[i]->buffer[2]=BYTE_TRAMA[7];           
          }	
        }       
      }
    }













  }
  
}

int8_t Interfaccia::interfaccia_send_COMANDO(uint8_t A, uint8_t PL,  uint8_t stato, uint8_t attendi_risposta){
	uint8_t checkbytes=0;
	uint8_t i=0;
	uint8_t address;
	unsigned long time1;
	uint8_t val=0;
	uint8_t old_stato;
	uint8_t zerobyte = 0 ;
	uint8_t bufferTxscs[10];

	old_stato = stato;

	address = 0;
	address = (A & 0x0F);
	address = address << 4;
	address = address & 0xF0;
	address = address | PL;

	for(i=0;i<8;i++){
		//Send Comando
		checkbytes=0;
		checkbytes = address;
		checkbytes = checkbytes ^ 0x00;
		checkbytes = checkbytes ^ 0x12;
		checkbytes = checkbytes ^ stato;
		
    bufferTxscs[0] = 0xA8;
    bufferTxscs[1] = address;
    bufferTxscs[2] = (uint8_t)0x00;
    bufferTxscs[3] = 0x12;
    bufferTxscs[4] = stato;
    bufferTxscs[5] = checkbytes;
    bufferTxscs[6] = 0xA3;
    scs->write(bufferTxscs,7);
/*
		scs->write(0xA8);
		scs->write(address);
		scs->write((uint8_t)0x00);
		scs->write(0x12);
		scs->write(stato);	//STATO
		scs->write(checkbytes);
		scs->write(0xA3);
*/	
    if(attendi_risposta==1){
      //Aspetta A5 x timeout 300mS
      time1 = millis();
      
      while((millis() - time1) < 300){
        while(scs->available()>0){
          val = scs->read();
          if(val == 0xA5){
            if(stato == 1 ){
              return 0;
            }else if(stato == 0 ){
              return 1;
            }else{
              return stato;
            }
          }
        }	
	#if defined(ESP8266)
		yield();
	#endif			
      }
	  
      delay(1);	 
    }else{
      if(i==2){
        if(stato==1){
          stato=0;
        }else{
          stato=1;
        }
          return stato;
      }
    }
  }
	return old_stato;
}


void Interfaccia::interfaccia_send_COMANDO_7_RAW(uint8_t* buffertx){
	uint8_t checkbytes=0;
	uint8_t i=0;

  checkbytes=0;
  checkbytes = buffertx[1];
  checkbytes = checkbytes ^ buffertx[2];
  checkbytes = checkbytes ^ buffertx[3];
  checkbytes = checkbytes ^ buffertx[4];
  buffertx[5] = checkbytes;

	for(i=0;i<3;i++){
		//Send Comando	
    scs->write(buffertx,7);
  }
}

void Interfaccia::interfaccia_send_COMANDO_11_RAW(uint8_t* buffertx){
	uint8_t checkbytes=0;
	uint8_t i=0;

  checkbytes=0;
  checkbytes = buffertx[1];
  checkbytes = checkbytes ^ buffertx[2];
  checkbytes = checkbytes ^ buffertx[3];
  checkbytes = checkbytes ^ buffertx[4];
  checkbytes = checkbytes ^ buffertx[5];
  checkbytes = checkbytes ^ buffertx[6];
  checkbytes = checkbytes ^ buffertx[7];
  checkbytes = checkbytes ^ buffertx[8];

  buffertx[9] = checkbytes;

	for(i=0;i<3;i++){
		//Send Comando	
    scs->write(buffertx,11);
  }
}



//*****************************************************************************************
//*****************************************************************************************
//*****************************************************************************************
//*****************************************************************************************
//                      IMPLEMENTAZIONE *TIPI* INTERFACCIE
//*****************************************************************************************
//*****************************************************************************************
//*****************************************************************************************
//*****************************************************************************************
    
//Switch, Attuatore!    
//Switch, Attuatore!    
//Switch, Attuatore!    
//Switch, Attuatore!    
    
Switch::Switch(Interfaccia* i){
  Set_Type(SWITCH);
  _interfaccia = i;

  _interfaccia->Add_Obj_Interface(this);
  Set_Stato(0);
  Reset_Change_Stato();
}
void Switch::On(void){
  uint8_t stato_rele=0;
    Serial.println("Accendo");

  stato_rele = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), 0, 1);
  Set_Stato(stato_rele);
  Reset_Change_Stato();
}
void Switch::Off(void){
  uint8_t stato_rele=1;

    Serial.println("Spegno");    

  stato_rele = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), 1, 1);
  Set_Stato(stato_rele);
  Reset_Change_Stato();
}
void Switch::Toggle(void){
  uint8_t stato_rele;
  stato_rele = 1;
  if(Get_Stato() == 1){
    stato_rele = 0;
  }
  stato_rele = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), stato_rele, 1);
  Set_Stato(stato_rele);
  Reset_Change_Stato();
}
uint8_t Switch::Stato(void){
  uint8_t stato_rele;
  stato_rele = 1;
  if(Get_Stato() == 1){
    stato_rele = 0;
  }
  return stato_rele;
}

    
//Dimmer, Attuatore!    
//Dimmer, Attuatore!    
//Dimmer, Attuatore!    
//Dimmer, Attuatore!    
    
Dimmer::Dimmer(Interfaccia* i){
  Set_Type(DIMMER);
  _interfaccia = i;

  _interfaccia->Add_Obj_Interface(this);
  Set_Stato(0);
  Reset_Change_Stato();
}
void Dimmer::On(void){
  uint8_t stato_rele=0;
    Serial.println("Accendo Dimmer");

  stato_rele = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), 0, 1);
  Set_Stato(stato_rele);
  Reset_Change_Stato();
}
void Dimmer::Off(void){
  uint8_t stato_rele=1;

    Serial.println("Spegno Dimmer");    

  stato_rele = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), 1, 1);
  Set_Stato(stato_rele);
  Reset_Change_Stato();
}
void Dimmer::Toggle(void){
  uint8_t stato_rele;
  uint8_t stato_dimmer = Get_Stato();
  if((stato_dimmer > 1)|(stato_dimmer == 0)){ 
    //Acceso con dimmerazione -> Spegni
    stato_rele = 1;
  }else{
    stato_rele = 0;
  }
  stato_rele = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), stato_rele, 1);
  Set_Stato(stato_rele);
  Reset_Change_Stato();
}
uint8_t Dimmer::Stato(void){
  uint8_t stato_rele;
  uint8_t stato_dimmer = Get_Stato();

  if(stato_dimmer > 1){ 
    //Acceso con dimmerazione
    stato_rele = stato_dimmer;
  }else if(stato_dimmer == 0){
    //Acceso 
    stato_rele = 1;
  }else{
    //Spento    == 1
    stato_rele = 0;
  }
  return stato_rele;
}
void Dimmer::dimmer_value(uint8_t percent){
  //0-100 in 10-80
  //uint8_t percentt = map(percent,0,100,10,80);
  if(percent > 80){
    percent = 80;
  }
  if(percent < 20){
    percent = 20;
  }
  uint8_t percentt = percent;



  uint8_t stato_rele=0;
  uint8_t stato_relex=0;

  uint8_t v = percentt / 10;
  uint8_t vd = _pValueDimmer[v];

  stato_relex = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), vd, 1);
  Set_Stato(vd);
  Reset_Change_Stato();
}

uint8_t Dimmer::Get_Percent(){
  uint8_t stato_rele;
  uint8_t percent=0;
  uint8_t stato_dimmer = Get_Stato();

  if(stato_dimmer == 0){
    //Acceso 
    stato_rele = 1;
  }else if(stato_dimmer == 1){
    //Spento    == 1
    stato_rele = 0;
  }else if(stato_dimmer > 1){ 
    //Acceso con dimmerazione
    //Converti in percentuale 0-100
    switch(stato_dimmer){
      case 0x0D:
        stato_rele = 10;
      break;
      case 0x1D:
        stato_rele = 20;
      break;
      case 0x2D:
        stato_rele = 30;
      break;
      case 0x3D:
        stato_rele = 40;
      break;
      case 0x4D:
        stato_rele = 50;
      break;
      case 0x5D:
        stato_rele = 60;
      break;
      case 0x6D:
        stato_rele = 70;
      break;
      case 0x7D:
        stato_rele = 80;
      break;
      case 0x8D:
        stato_rele = 90;
      break;
      case 0x9D:
        stato_rele = 100;
      break;
      default:
        stato_rele = 10;
      break;
    }
  }
  return stato_rele;
}


//Serranda, Attuatore!    
//Serranda, Attuatore!    
//Serranda, Attuatore!    
//Serranda, Attuatore!    
//Serranda, Attuatore!    

Serranda::Serranda(Interfaccia* i){
  Set_Type(SERRANDA);
  _interfaccia = i;

  _interfaccia->Add_Obj_Interface(this);
  Set_Stato(0);
  Reset_Change_Stato();

  timer_flag = 0;
  stato_percentuale = 0;
  buffer[0] = 0;
  buffer[5] = 0;
  _STATO = 0;
  precenttualeChange = 1;
}
void Serranda::Alza(void){
  uint8_t stato_rele=0;

  stato_rele = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), 0x0A, 1);
  if(stato_rele == 0x0A){
    delay(500);
    stato_rele = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), 0x08, 1);
  }
  Set_Stato(stato_rele);
  Reset_Change_Stato();
  if(timer_flag==0){
    stato_percentuale = 100;    //100%
    precenttualeChange = 1;
    Serial.println("finestra Alzo Reset Reset percetuale");
  }
}
void Serranda::Abbassa(void){
  uint8_t stato_rele=0;
  stato_rele = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), 0x0A, 1);

  if(stato_rele == 0x0A){
    delay(500);
    stato_rele = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), 0x09, 1);
  }
  Set_Stato(stato_rele);
  Reset_Change_Stato();
  if(timer_flag==0){
    stato_percentuale = 0;    //0%
    precenttualeChange = 1;
    Serial.println("finestra Abbasso Reset percetuale");
  }
}
void Serranda::Stop(void){
  uint8_t stato_rele=0;

  stato_rele = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), 0x0A, 1);
}


void Serranda::Toggle(void){
  uint8_t stato_rele;

  stato_rele = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), 0x0A, 1);
  if(stato_rele == 0x0A){
    delay(500);
    stato_rele = 0x08;
    if(Get_Stato() == 0x08){
      stato_rele = 0x09;
    }
    stato_rele = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), stato_rele, 1);
    Set_Stato(stato_rele);
    Reset_Change_Stato();
  } 
}


void Serranda::set_Timer(unsigned long timer_salita, unsigned long timer_discesa){
  timer_salita_ = timer_salita;
  timer_discesa_ = timer_discesa;
}

void Serranda::Alza(int value_percent){
  if(value_percent == 100){
    timer_flag=0;
    Alza();
  }else{
    timer_flag = 1;
    value_percent = abs(value_percent);

    calcolo_stop_ = (timer_salita_ / 100) * value_percent;

    Serial.print("Serranda::Alza: ");
    Serial.println(calcolo_stop_);

    Alza();
    TIMER_ = millis();
  }
}
void Serranda::Abbassa(int value_percent){
  if(value_percent == 0){
    timer_flag=0;
    Abbassa();
  }else{  
    timer_flag = -1;
    int value_percentp = abs(value_percent);

    calcolo_stop_ = (timer_discesa_ / 100) * value_percentp;

    Serial.print("Serranda::Abbassa: ");
    Serial.println(calcolo_stop_);

    Abbassa();
    TIMER_ = millis();
  }
}

void Serranda::Reset_timer_flag(void){
  timer_flag=0;
}

void Serranda::action(int value_percent){
   //stato_percentuale //stato attuale
  Serial.print("Serranda action percentuale: ");
  Serial.println(value_percent);


  int action = value_percent - stato_percentuale;

  Serial.print("Serranda action: ");
  Serial.println(action);

  if(action < 0){
    Abbassa(action);
  }else if(action > 0){
    Alza(action);
  }
  //stato_percentuale = value_percent;
  
  //Serial.print("Serranda stato [percentuale]: ");
  //Serial.println(stato_percentuale);

}

int Serranda::timer(void){

  if(timer_flag==1){
    //Salita
    if((millis()-TIMER_)>=calcolo_stop_){
      unsigned long savetime_esplaw = (millis()-TIMER_);

      Stop();

      unsigned long calcoloperc = (100.0 / (unsigned long)timer_salita_) * savetime_esplaw;
      Serial.print("Serranda stato [calcoloperc]: ");
      Serial.print(calcoloperc);
      Serial.print("  ");
      Serial.print(timer_salita_);
      Serial.print(millis());
      Serial.print("  ");
      Serial.print(TIMER_);
      Serial.print("  ");
      Serial.println(savetime_esplaw);
      

      int deltaPercentuale = (int)calcoloperc;
      stato_percentuale = stato_percentuale + deltaPercentuale;
      if(stato_percentuale > 100){
        stato_percentuale = 100;
      }

      Serial.println("Serranda::STOP");

      Serial.print("Serranda stato [percentuale]: ");
      Serial.println(stato_percentuale);

      timer_flag = 0;

      buffer[5] = 0;
      buffer[0] = 0;
      _STATO = 0;
      precenttualeChange = 1;
    }
  }else if(timer_flag == -1){
    //Discesa
    if((millis()-TIMER_)>=calcolo_stop_){
      unsigned long savetime_esplaw = (millis()-TIMER_);

      Stop();

      unsigned long calcoloperc = (100.0 / (unsigned long)timer_discesa_) * savetime_esplaw;
      Serial.print("Serranda stato [calcoloperc]: ");
      Serial.print(calcoloperc);
      Serial.print("  ");
      Serial.print(timer_salita_);
      Serial.print(millis());
      Serial.print("  ");
      Serial.print(TIMER_);
      Serial.print("  ");
      Serial.println(savetime_esplaw);


      int deltaPercentuale = (int)calcoloperc;
      stato_percentuale = stato_percentuale - deltaPercentuale;
      if(stato_percentuale < 0){
        stato_percentuale = 0;
      }

      Serial.println("Serranda::STOP");

      Serial.print("Serranda stato [percentuale]: ");
      Serial.println(stato_percentuale);

      timer_flag = 0;

      buffer[5] = 0;
      buffer[0] = 0;
      _STATO = 0;
      precenttualeChange = 1;
    }
  }



  switch(_STATO){
    case 0:
      if(buffer[0] == 2){
        buffer[5] = 2;
        buffer[0] = 0;
        //Comando Alza manuale
        convertByteArryTo32.buffer[0] = buffer[1];
        convertByteArryTo32.buffer[1] = buffer[2];
        convertByteArryTo32.buffer[2] = buffer[3];
        convertByteArryTo32.buffer[3] = buffer[4];     
        timeSTART = convertByteArryTo32.val;
        Serial.print("Time START Comando Alza: ");
        Serial.println(timeSTART);

        _STATO = 1;
      }

      if(buffer[0] == 1){
        buffer[5] = 1;
        buffer[0] = 0;
        //Comando Abbassa manuale
        convertByteArryTo32.buffer[0] = buffer[1];
        convertByteArryTo32.buffer[1] = buffer[2];
        convertByteArryTo32.buffer[2] = buffer[3];
        convertByteArryTo32.buffer[3] = buffer[4];     
        timeSTART = convertByteArryTo32.val;
        Serial.print("Time START Comando Abbassa: ");
        Serial.println(timeSTART);

        _STATO = 1;
      }
    break;

    case 1:

      if(buffer[0] == 3){
        buffer[0] = 0;
        //Comando STOP manuale
        convertByteArryTo32.buffer[0] = buffer[1];
        convertByteArryTo32.buffer[1] = buffer[2];
        convertByteArryTo32.buffer[2] = buffer[3];
        convertByteArryTo32.buffer[3] = buffer[4];     
        unsigned long timeSTOP = convertByteArryTo32.val;
        unsigned long timeelapsed = timeSTOP - timeSTART;

        int deltaPercentuale;
        if(buffer[5] == 2){
          float calcoloperc = (100.0 / (float)timer_salita_) * (float)timeelapsed;
          deltaPercentuale = (int)calcoloperc;
          stato_percentuale = stato_percentuale + deltaPercentuale;
          if(stato_percentuale > 100){
            stato_percentuale = 100;
          }
        }else{
          float calcoloperc = (100.0 / (float)timer_discesa_) * (float)timeelapsed;
          deltaPercentuale = (int)calcoloperc;
          stato_percentuale = stato_percentuale - deltaPercentuale;
          if(stato_percentuale < 0){
            stato_percentuale = 0;
          }
        }

        Serial.print("Time STOP Comando [T trascorso]: ");
        Serial.print(timeelapsed);
        Serial.print("  DELTA percentuale: ");
        Serial.print(deltaPercentuale);
        Serial.print("  percentuale: ");
        Serial.println(stato_percentuale);



        _STATO = 0;
        buffer[5] = 0;
        precenttualeChange = 1;
      }

      if(buffer[5] == 2){
          //Comando precedente alza
        unsigned long timeelapsed = millis() - timeSTART;
        if(timeelapsed >= timer_salita_){
            //fine corsa
          buffer[0] = 0;
          buffer[0] = 0;
          _STATO = 0;
          stato_percentuale = 100;
          precenttualeChange = 1;

          Serial.println("Time Fine corsa % 100");
        }
      }
      if(buffer[5] == 1){
          //Comando precedente Abbassa
        unsigned long timeelapsed = millis() - timeSTART;
        if(timeelapsed >= timer_discesa_){
            //fine corsa
          buffer[0] = 0;
          buffer[0] = 0;
          _STATO = 0;
          stato_percentuale = 0;
          precenttualeChange = 1;
          Serial.println("Time Fine corsa % 0");
        }
      }
    break;
  }


  return precenttualeChange;  

}




//Gruppo, Attuatori!
//Gruppo, Attuatori!
//Gruppo, Attuatori!
//Gruppo, Attuatori!
//Gruppo, Attuatori!

GruppoSwitch::GruppoSwitch(Interfaccia* i){
  Set_Type(GRUPPO);
  _interfaccia = i;

  _interfaccia->Add_Obj_Interface(this);
  Set_Stato(0);
  Reset_Change_Stato();
}
void GruppoSwitch::On(void){
  uint8_t stato_rele=0;

  stato_rele = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), 0, 0);
  Set_Stato(stato_rele);
  Reset_Change_Stato();
}
void GruppoSwitch::Off(void){
  uint8_t stato_rele=1;

  stato_rele = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), 1, 0);
  Set_Stato(stato_rele);
  Reset_Change_Stato();
}
void GruppoSwitch::Toggle(void){
  uint8_t stato_rele;
  stato_rele = 1;
  if(Get_Stato() == 1){
    stato_rele = 0;
  }
  stato_rele = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), stato_rele, 0);
  Set_Stato(stato_rele);
  Reset_Change_Stato();
}



    
//Serratura, Attuatore!    
//Serratura, Attuatore!    
//Serratura, Attuatore!    
//Serratura, Attuatore!    
    
Serratura::Serratura(Interfaccia* i){
  Set_Type(SERRATURA);
  _interfaccia = i;

  _interfaccia->Add_Obj_Interface(this);
  Set_Stato(0);
  Reset_Change_Stato();
}
void Serratura::Sblocca(void){
  //Invia comando x requet temp
    uint8_t bufferTxscs[7];
    bufferTxscs[0] = 0xA8;
    bufferTxscs[1] = 0x96;
    bufferTxscs[2] = 0xA0 | Get_Address_A();
    bufferTxscs[3] = 0x6F;
    bufferTxscs[4] = 0xA4;
    bufferTxscs[5] = 0;   // lo calcola interfaccia_send_COMANDO_7_RAW
    bufferTxscs[6] = 0xA3;
  _interfaccia->interfaccia_send_COMANDO_7_RAW(bufferTxscs);

  Reset_Change_Stato();
}


//Campanello, Attuatore!    
//Campanello, Attuatore!    
//Campanello, Attuatore!    
//Campanello, Attuatore!    
    
Campanello::Campanello(Interfaccia* i){
  Set_Type(CAMPANELLO);
  _interfaccia = i;

  _interfaccia->Add_Obj_Interface(this);
  Set_Stato(0);
  Reset_Change_Stato();

  TIMERCAMPANELLO = millis();
}

uint8_t  Campanello::is_pressed(void){
  if(Get_Stato() == 1){
    if((millis()-TIMERCAMPANELLO) > 1500){
      TIMERCAMPANELLO = millis();

      Set_Stato(0);
      return 1;
    }else{
      Set_Stato(0);
      return 0;
    }
    //Set_Stato(0);
    //return 1;
  }
  return 0;
}




//Temperature Sensori!
//Temperature Sensori!
//Temperature Sensori!
//Temperature Sensori!
//Temperature Sensori!

TemperatureSensor::TemperatureSensor(Interfaccia* i){
  Set_Type(TEMPERATURE_SENSOR);
  _interfaccia = i;

  _interfaccia->Add_Obj_Interface(this);
  Set_Stato(0);
  Reset_Change_Stato();
}
bool TemperatureSensor::available(void){
  if(Get_Stato() == 1){
    Set_Stato(0);
    Reset_Change_Stato();
    return true;
  }
  return false;
}
float TemperatureSensor::Get(void){
  float temp=0.000;

  temp = buffer[0];
  temp = temp / 10;
  return temp;
}
void TemperatureSensor::Request(void){
  //Invia comando x requet temp
    uint8_t bufferTxscs[7];
    bufferTxscs[0] = 0xA8;
    bufferTxscs[1] = 0x99;
    bufferTxscs[2] = Get_Address_A();
    bufferTxscs[3] = 0x30;
    bufferTxscs[4] = 0x00;
    bufferTxscs[5] = 0;   // lo calcola interfaccia_send_COMANDO_7_RAW
    bufferTxscs[6] = 0xA3;
  _interfaccia->interfaccia_send_COMANDO_7_RAW(bufferTxscs);
}




//Thermostat Sensori!
//Thermostat Sensori!
//Thermostat Sensori!
//Thermostat Sensori!
//Thermostat Sensori!

Thermostat::Thermostat(Interfaccia* i){
  Set_Type(THERMOSTAT);
  _interfaccia = i;

  _interfaccia->Add_Obj_Interface(this);
  Set_Stato(0);
  Reset_Change_Stato();
 
  temperatureSensor = new TemperatureSensor(i);
}
bool Thermostat::change(void){
  if(Get_Stato() == 1){
    Set_Stato(0);
    Reset_Change_Stato();
    return true;
  }
  return false;
}
float Thermostat::Get(void){
  float temp=0.000;

  temp = buffer[0];
  temp = temp / 10;
  return temp;
}
void Thermostat::Request_Stato(void){
  //Invia comando x requet STATO
    uint8_t bufferTxscs[7];
    bufferTxscs[0] = 0xA8;
    bufferTxscs[1] = 0x99;
    bufferTxscs[2] = Get_Address_A();
    bufferTxscs[3] = 0x30;
    bufferTxscs[4] = 0x11;
    bufferTxscs[5] = 0;   // lo calcola interfaccia_send_COMANDO_7_RAW
    bufferTxscs[6] = 0xA3;
  _interfaccia->interfaccia_send_COMANDO_7_RAW(bufferTxscs);
}
void Thermostat::Request_Temp_Setting(void){
  //Invia comando x requet Temperatura di Setting
    uint8_t bufferTxscs[7];
    bufferTxscs[0] = 0xA8;
    bufferTxscs[1] = 0x99;
    bufferTxscs[2] = Get_Address_A();
    bufferTxscs[3] = 0x30;
    bufferTxscs[4] = 0x0E;
    bufferTxscs[5] = 0;   // lo calcola interfaccia_send_COMANDO_7_RAW
    bufferTxscs[6] = 0xA3;
  _interfaccia->interfaccia_send_COMANDO_7_RAW(bufferTxscs);
}

uint8_t Thermostat::loop(void){
  if(temperatureSensor->available()){
    temperature = temperatureSensor->Get(); 

    Serial.print("[Termostato] sensore temperatura: ");
    Serial.println(temperature);

    _avaiable = 1;
    return 1;
  }
  if(Get_Stato() == 1 ){
    Set_Stato(0);
    //Temeratura di setting cambiata
    temperature_di_Setting = ((buffer[0] - 6) * 0.50) + 3;

    Serial.print("[Termostato] setting temperatura 1: ");
    Serial.println(temperature_di_Setting);

    _avaiable = 2;
    return 2;
  }
  if(Get_Stato() == 2 ){
    Set_Stato(0);

    uint16_t v = 0;
    v = buffer[3] << 8;
    v = v | buffer[4];
    temperature_di_Setting = (v / 10.0);

    Serial.print("[Termostato] setting temperatura 3: ");
    Serial.println(temperature_di_Setting);


    //Modalita cambiata
    if(((buffer[1] & 0x0F) == 0x02) | ((buffer[1] & 0x0F) == 0x00)){
      //Inverno

      Serial.print("[Termostato] modalita: ");
      Serial.println("Inverno");

      _avaiable = 4;
      return 4;

    }
    if(((buffer[1] & 0x0F) == 0x03) | ((buffer[1] & 0x0F) == 0x01)){
      //Estate

      Serial.print("[Termostato] modalita: ");
      Serial.println("Estate");

      _avaiable = 3;
      return 3;
    }
    if(buffer[1] == 0xFF){
      //OFF

      Serial.print("[Termostato] modalita: ");
      Serial.println("OFF");

      _avaiable = 5;
      return 5;
    }

  }
  if(Get_Stato() == 3 ){
    Set_Stato(0);
    //Temeratura di setting cambiata
    temperature_di_Setting = (buffer[2] / 10.0);

    Serial.print("[Termostato] setting temperatura 2: ");
    Serial.println(temperature_di_Setting);

    _avaiable = 2;
    return 2;
  }


  return 0;
}


void Thermostat::set_temperature(float t){
    uint8_t val = (((t - 3) / 0.5) + 6);
    /*
    Serial.print("Temperatura impostata: ");
    Serial.print(t);
    Serial.print("  ;  ");
    Serial.println(val,HEX);
    */

    uint8_t bufferTxscs[11];
    bufferTxscs[0] = 0xA8;
    bufferTxscs[1] = 0xD1;
    bufferTxscs[2] = Get_Address_A();
    bufferTxscs[3] = 0x03;
    bufferTxscs[4] = 0x02;
    bufferTxscs[5] = 0xC1;
    bufferTxscs[6] = 0x02;
    bufferTxscs[7] = val;    //Temp
    bufferTxscs[8] = 0x00;
    bufferTxscs[9] = 0x00;   // lo calcola interfaccia_send_COMANDO_11_RAW
    bufferTxscs[10] = 0xA3;

  _interfaccia->interfaccia_send_COMANDO_11_RAW(bufferTxscs);
}

void Thermostat::set_mode(uint8_t m){
    uint8_t bufferTxscs[11];

  if(m==0){
    //OFF
    bufferTxscs[0] = 0xA8;
    bufferTxscs[1] = 0xD1;
    bufferTxscs[2] = Get_Address_A();
    bufferTxscs[3] = 0x03;
    bufferTxscs[4] = 0x02;
    bufferTxscs[5] = 0xC1;
    bufferTxscs[6] = 0x06;
    bufferTxscs[7] = 0x00;    
    bufferTxscs[8] = 0x00;
    bufferTxscs[9] = 0x00;   // lo calcola interfaccia_send_COMANDO_11_RAW
    bufferTxscs[10] = 0xA3;
  _interfaccia->interfaccia_send_COMANDO_11_RAW(bufferTxscs);  
  }else if (m==1)
  {
    //freddo

    bufferTxscs[0] = 0xA8;
    bufferTxscs[1] = 0xD1;
    bufferTxscs[2] = Get_Address_A();
    bufferTxscs[3] = 0x03;
    bufferTxscs[4] = 0x02;
    bufferTxscs[5] = 0xC1;
    bufferTxscs[6] = 0xF1;
    bufferTxscs[7] = 0x00;    
    bufferTxscs[8] = 0x00;
    bufferTxscs[9] = 0x00;   // lo calcola interfaccia_send_COMANDO_11_RAW
    bufferTxscs[10] = 0xA3;
  _interfaccia->interfaccia_send_COMANDO_11_RAW(bufferTxscs);  
  }else if (m==2)
  {
    //caldo
    bufferTxscs[0] = 0xA8;
    bufferTxscs[1] = 0xD1;
    bufferTxscs[2] = Get_Address_A();
    bufferTxscs[3] = 0x03;
    bufferTxscs[4] = 0x02;
    bufferTxscs[5] = 0xC1;
    bufferTxscs[6] = 0xF0;
    bufferTxscs[7] = 0x00;    
    bufferTxscs[8] = 0x00;
    bufferTxscs[9] = 0x00;   // lo calcola interfaccia_send_COMANDO_11_RAW
    bufferTxscs[10] = 0xA3;
  _interfaccia->interfaccia_send_COMANDO_11_RAW(bufferTxscs);  
  }else if (m==3)
  {
  }
  
  
  


}
