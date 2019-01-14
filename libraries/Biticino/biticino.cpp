/*
 * biticino.cpp
 *
 * Created: 27/12/2018 10:08:08
 *  Author: michele
 */ 

#include "biticino.h"

Biticino::Biticino(int rx, int tx){
	_rx = rx;
	_tx = tx;
	
	#if defined(__AVR__)
		scs = new SoftwareSerial(_rx,_tx);
	#elif defined(ESP8266)
		scs = new EspSoftwareSerial(_rx, _tx, false, 256);
	#endif	
	
	STATE_MACHINE_Read_TRAMA = 0;
}

void Biticino::begin(void){
	scs->begin(9600);
}




int8_t Biticino::attuatore(int8_t A){
	int8_t val;
	uint8_t add;
	int8_t result = 0;

	val = search_TRAMA(0xA8, 0xA3);
	if(val == 1) {
		//Trama TROVATA
		if(BYTE_TRAMA[3] == 0x12){	//Richiesta Comando

			Serial.println("*----> richiesta comando*");				
	
			//Codifica indirizzi e verifica indirizzi
			//A
			add = 0;
			add = (A & 0x0F);
			add = add << 4;
			add = add & 0xF0;
			
			Serial.print("add = ");				
			Serial.println(add, HEX);				
			
			if(add == (BYTE_TRAMA[1] & 0xF0)){	//A corisponde
				_indirizzo = BYTE_TRAMA[1] & 0x0F;
				_comando = BYTE_TRAMA[4];
				result = 1;

				//Invia Risposta ACK
				attuatore_send_ACK(0xA5);
				
				// Invia 3 volte comando B8 
				//attuatore_send_STATO(BYTE_TRAMA[1], _comando);
				
				
			}	
		}
		if(BYTE_TRAMA[3] == 0x15){	//Richiesta Stato

			Serial.println("*----> richiesta stato*");				
		
			//Codifica indirizzi e verifica indirizzi
			//A
			add = 0;
			add = (A & 0x0F);
			add = add << 4;
			add = add & 0xF0;

			Serial.print("add = ");				
			Serial.println(add, HEX);				
			
			if(add == (BYTE_TRAMA[1] & 0xF0)){	//A corisponde
				//Invia Telegramma Di STATO
				_indirizzo = BYTE_TRAMA[1] & 0x0F;
				result = 2;
			}
		}

	}
	
	return result;
}

uint8_t Biticino::Get_Comando(void){
	return _comando;
}
uint8_t Biticino::Get_Indirizzo(void){
	return _indirizzo;
}

void Biticino::attuatore_send_ACK(uint8_t ack){

	Serial.print("SEND ACK = ");				
	Serial.println(ack, HEX);				

	scs->write(ack);
}
void Biticino::attuatore_send_STATO(uint8_t address, uint8_t stato){
	uint8_t checkbytes=0;
	uint8_t i=0;

	for(i=0;i<3;i++){
		checkbytes=0;
		checkbytes = 0xB8 ^ address;
		checkbytes = checkbytes ^ 0x12;
		checkbytes = checkbytes ^ stato;

		Serial.println("SEND STATO");
		Serial.print("          add= ");
		Serial.println(address, HEX);
		Serial.print("          stato= ");
		Serial.println(stato, HEX);
		Serial.print("          checkbytes= ");
		Serial.println(checkbytes, HEX);
		
		scs->write(0xA8);
		scs->write(0xB8);
		scs->write(address);	//ADDRESS
		scs->write(0x12);
		scs->write(stato);	//STATO
		scs->write(checkbytes);
		scs->write(0xA3);
		
		delay(1);
	}
}
void Biticino::attuatore_send_STATO(uint8_t A, uint8_t PL,  uint8_t stato){
	uint8_t address;
	address = 0;
	address = (A & 0x0F);
	address = address << 4;
	address = address & 0xF0;
	address = address | PL;

	attuatore_send_STATO(address, stato);
}




int8_t Biticino::search_TRAMA(uint8_t START, uint8_t FINISH){
	uint8_t val;
	uint8_t res;
	int8_t risultato = 0;

	while(scs->available()>0){
		
//		Serial.print("[SERIAL_AVAIABLE]: ");
		
		val = scs->read();

		Serial.println(val, HEX);
		
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
				//FIND A3
				if(val == FINISH){
					BYTE_TRAMA[6] = val;
					//Verifica se corretto
					res = BYTE_TRAMA[1] ^ BYTE_TRAMA[2];
					res = res ^ BYTE_TRAMA[3];
					res = res ^ BYTE_TRAMA[4];
					if(res == BYTE_TRAMA[5]){
						//Corretto !!!
						risultato = 1;
						
						//Serial.println("-STATE_MACHINE_Read_TRAMA-  *TRAMA TROVATA - CORRETTO-*");				
					}
				}
				STATE_MACHINE_Read_TRAMA = 0;
			break;
		}
		
		
		if(risultato==1){
			break;			
		}
	}
	
	return risultato;
}



//*******************************************************************************************************************
//*******************************************************************************************************************
//*******************************************************************************************************************
//*******************************************************************************************************************
//*******************************************************************************************************************
//*******************************************************************************************************************
//*******************************************************************************************************************
//*******************************************************************************************************************
//					I N T E R F A C C I A
//*******************************************************************************************************************
//*******************************************************************************************************************
//*******************************************************************************************************************
//*******************************************************************************************************************


int8_t Biticino::interfaccia_send_COMANDO(uint8_t A, uint8_t PL,  uint8_t stato){
	uint8_t checkbytes=0;
	uint8_t i=0;
	uint8_t address;
	unsigned long time1;
	uint8_t val=0;
	uint8_t old_stato;
	uint8_t zerobyte = 0 ;
	
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

		Serial.println("SEND COMANDO");
		Serial.print("          add= ");
		Serial.println(address, HEX);
		Serial.print("          stato= ");
		Serial.println(stato, HEX);
		Serial.print("          checkbytes= ");
		Serial.println(checkbytes, HEX);
		
		scs->write(0xA8);
		scs->write(address);
		scs->write((uint8_t)0x00);
		scs->write(0x12);
		scs->write(stato);	//STATO
		scs->write(checkbytes);
		scs->write(0xA3);
		
	
		//Aspetta A5 x timeout 300mS
		time1 = millis();
		
		while((millis() - time1) < 300){
			while(scs->available()>0){
				val = scs->read();

				Serial.println(val, HEX);
				
				if(val == 0xA5){

					Serial.println("SEND COMANDO------ <<<SUCCESS>>>");
				
					if(stato == 1 ){
						return 0;
						
					}else{
						return 1;
					}
				}
			}
		}
		
		delay(1);
	
	}

	return old_stato;
}







int8_t Biticino::attuatore_check_cambio_stato(uint8_t A){
	int8_t val;
	uint8_t add;
	int8_t result = 0;

	val = search_TRAMA(0xA8, 0xA3);
	if(val == 1) {
		//Trama TROVATA
		if(BYTE_TRAMA[1] == 0xB8){	//Richiesta Comando

			Serial.println("*----> attuatore averte cambio stato*");				
	
			//Codifica indirizzi e verifica indirizzi
			//A
			add = 0;
			add = (A & 0x0F);
			add = add << 4;
			add = add & 0xF0;
			
			Serial.print("add = ");				
			Serial.println(add, HEX);				
			
			if(add == (BYTE_TRAMA[2] & 0xF0)){	//A corisponde
				_indirizzo = BYTE_TRAMA[2] & 0x0F;
				_comando = BYTE_TRAMA[4];
				result = 1;
			}	
		}
	}
	return result;
}
