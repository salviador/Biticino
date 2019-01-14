/*
 * biticino.h
 *
 * Created: 27/12/2018 10:08:21
 *  Author: michele
 */ 


#ifndef BITICINO_H_
#define BITICINO_H_

	#include <Arduino.h>

	#if defined(__AVR__)
		#include "SoftwareSerial.h"
	#elif defined(ESP8266)
		#include "espSoftwareSerial.h"
	#endif




	 class Biticino
	 {
		#define LEN_TRAMA_SCS_BUS	7

		 public:
			Biticino(int rx, int tx);
			void begin(void);

			
			int8_t attuatore(int8_t A);
			uint8_t Get_Comando(void);
			uint8_t Get_Indirizzo(void);

			void attuatore_send_ACK(uint8_t ack);
			void attuatore_send_STATO(uint8_t A, uint8_t PL,  uint8_t stato);
			void attuatore_send_STATO(uint8_t address, uint8_t stato);

			
			int8_t interfaccia_send_COMANDO(uint8_t A, uint8_t PL,  uint8_t stato);
			int8_t attuatore_check_cambio_stato(uint8_t A);


		 private:

			volatile uint8_t _comando;
			volatile uint8_t _indirizzo;

			volatile uint8_t BYTE_TRAMA[LEN_TRAMA_SCS_BUS];
			volatile uint8_t STATE_MACHINE_Read_TRAMA;


			int _rx;
			int _tx;
    
			#if defined(__AVR__)
				SoftwareSerial *scs;
			#elif defined(ESP8266)
				EspSoftwareSerial *scs;
			#endif
  			
			
			
			
			
			int8_t search_TRAMA(uint8_t START, uint8_t FINISH);

	 };



#endif /* BITICINO_H_ */