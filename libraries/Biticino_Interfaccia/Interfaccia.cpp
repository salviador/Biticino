#include "Interfaccia.h"






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
//		Serial.print("[SERIAL_AVAIABLE]: ");
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

        if((BYTE_TRAMA[3]==0xFF)&&(BYTE_TRAMA[4]==0x0F)){
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
          }	
        }
      }

		}
	}else if(val == 2){
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
}
void Switch::On(void){
  uint8_t stato_rele=0;

  stato_rele = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), 0, 1);
  Set_Stato(stato_rele);
}
void Switch::Off(void){
  uint8_t stato_rele=1;

  stato_rele = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), 1, 1);
  Set_Stato(stato_rele);
}
void Switch::Toggle(void){
  uint8_t stato_rele;
  stato_rele = 1;
  if(Get_Stato() == 1){
    stato_rele = 0;
  }
  stato_rele = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), stato_rele, 1);
  Set_Stato(stato_rele);
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
}
void Serranda::Salita(void){
  uint8_t stato_rele=0;

  stato_rele = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), 0x0A, 1);
  if(stato_rele == 0x0A){
    delay(500);
    stato_rele = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), 0x08, 1);
  }
  Set_Stato(stato_rele);
}
void Serranda::Discesa(void){
  uint8_t stato_rele=0;

  stato_rele = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), 0x0A, 1);
  if(stato_rele == 0x0A){
    delay(500);
    stato_rele = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), 0x09, 1);
  }
  Set_Stato(stato_rele);
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
  } 
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
}
void GruppoSwitch::On(void){
  uint8_t stato_rele=0;

  stato_rele = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), 0, 0);
  Set_Stato(stato_rele);
}
void GruppoSwitch::Off(void){
  uint8_t stato_rele=1;

  stato_rele = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), 1, 0);
  Set_Stato(stato_rele);
}
void GruppoSwitch::Toggle(void){
  uint8_t stato_rele;
  stato_rele = 1;
  if(Get_Stato() == 1){
    stato_rele = 0;
  }
  stato_rele = _interfaccia->interfaccia_send_COMANDO(Get_Address_A(), Get_Address_PL(), stato_rele, 0);
  Set_Stato(stato_rele);
}

