
#ifndef INTERFACCIA_H_
#define INTERFACCIA_H_

	    
	#include <Arduino.h>

	#if defined(__AVR__)
		#include "SoftwareSerial.h"
	#elif defined(ESP8266)
		#include "espSoftwareSerial.h"
	#endif



enum TYPE_INTERfACCIA {
  SWITCH,
  SERRANDA,
  GRUPPO
};
typedef enum TYPE_INTERfACCIA TYPE_INTERfACCIA_t ;





class abstractinterface
{
  public:
    void Set_Type(TYPE_INTERfACCIA_t t){
      _type = t;
    }
    TYPE_INTERfACCIA_t Get_Type(){
      return _type;
    }    
    //ADDRESS
    void Set_Address_A(uint8_t A){
      Address_A=A;
    }
    void Set_Address_PL(uint8_t PL){
      Address_PL=PL;
    }
    void Set_Address(uint8_t A , uint8_t PL){
      Address_A=A;       
      Address_PL=PL;
    }
    uint8_t Get_Address_A() const{
      return Address_A;
    }
    uint8_t  Get_Address_PL() const{
      return Address_PL;
    } 
    uint8_t  Get_Address() const{
      return (Address_A<<4) | Address_PL;
    } 

    virtual void address(uint8_t A, uint8_t PL)=0;

    void Set_Stato(uint8_t stato){
      _statoInterfaccia=stato;
    }
    uint8_t Get_Stato() const{
      return _statoInterfaccia;
    }

    
  private:
    TYPE_INTERfACCIA_t _type;

    uint8_t Address_A;
    uint8_t Address_PL;

    uint8_t _statoInterfaccia;

};




class Interfaccia
{
	#define LEN_TRAMA_SCS_BUS	7

  public:
    Interfaccia(int rx, int tx);
		void begin(void);

    
  
    void Loop_Seriale();
    
    int8_t interfaccia_send_COMANDO(uint8_t A, uint8_t PL,  uint8_t stato, uint8_t attendi_risposta);
   
    
    void Add_Obj_Interface(abstractinterface *i){
      _interfacee[_ctn_interfacee]=i;    
      _ctn_interfacee++;
    }
    
  private:
    int _ctn_interfacee;
    abstractinterface* _interfacee[20];


    //Gestione Seriale
		int8_t search_TRAMA(uint8_t START, uint8_t FINISH);

    volatile uint8_t BYTE_TRAMA[12];
    volatile uint8_t STATE_MACHINE_Read_TRAMA;

    int _rx;
    int _tx;
  
    #if defined(__AVR__)
      SoftwareSerial *scs;
    #elif defined(ESP8266)
      EspSoftwareSerial *scs;
    #endif

};



//*****************************************************************************************
//*****************************************************************************************
//*****************************************************************************************
//*****************************************************************************************
//                      IMPLEMENTAZIONE *TIPI* INTERFACCIE
//*****************************************************************************************
//*****************************************************************************************
//*****************************************************************************************
//*****************************************************************************************



class Switch : public abstractinterface
{
  public:
  
    Switch(Interfaccia* i);

    //COMANDI  
    void On();
    void Off();
    void Toggle();

    void address(const uint8_t A, const uint8_t PL){
      Set_Address_A(A);
      Set_Address_PL(PL);
    }
    void address(const uint8_t A_PL){
      uint8_t addt;
      addt = A_PL & 0xF0;
      addt = addt >> 4;
      addt = addt & 0x0F;
      Set_Address_A(addt);
      addt = A_PL & 0x0F;
      Set_Address_PL(addt);
    }


  private:
    Interfaccia* _interfaccia; 

};

class Serranda : public abstractinterface
{
  public:

    Serranda(Interfaccia* i);

    //COMANDI
    void Alza();
    void Abbassa();
    void Toggle();
    void Stop();

    void address(const uint8_t A, const uint8_t PL){
      Set_Address_A(A);
      Set_Address_PL(PL);
    }
    void address(const uint8_t A_PL){
      uint8_t addt;
      addt = A_PL & 0xF0;
      addt = addt >> 4;
      addt = addt & 0x0F;
      Set_Address_A(addt);
      addt = A_PL & 0x0F;
      Set_Address_PL(addt);
    }

  
  private:
    Interfaccia* _interfaccia;

};

class GruppoSwitch : public abstractinterface
{
  public:

    GruppoSwitch(Interfaccia* i);

    //COMANDI  
    void On();
    void Off();
    void Toggle();


    void address(const uint8_t A, const uint8_t PL){
      Set_Address_A(A);
      Set_Address_PL(PL);
    }
    void address(const uint8_t A_PL){
      uint8_t addt;
      addt = A_PL & 0xF0;
      addt = addt >> 4;
      addt = addt & 0x0F;
      Set_Address_A(addt);
      addt = A_PL & 0x0F;
      Set_Address_PL(addt);
    }

  private:
    Interfaccia* _interfaccia;
};













	
	
#endif /* BITICINO_H_ */