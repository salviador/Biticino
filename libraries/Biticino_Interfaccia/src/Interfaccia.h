
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
  GRUPPO,
  TEMPERATURE_SENSOR,
  THERMOSTAT,
  SERRATURA,
  CAMPANELLO
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
      _CHANGEstatoInterfaccia = true;
    }
    uint8_t Get_Stato() {
      //_CHANGEstatoInterfaccia = false;
      return _statoInterfaccia;
    }
    bool Is_Change_Stato(){
      if(_CHANGEstatoInterfaccia){
        _CHANGEstatoInterfaccia = false;  //Reset

        return true;
      }
      return false;
    }
    void Reset_Change_Stato() {
        _CHANGEstatoInterfaccia = false;  //Reset
    }
    
    uint8_t buffer[7];

  private:
    TYPE_INTERfACCIA_t _type;

    uint8_t Address_A;
    uint8_t Address_PL;

    uint8_t _statoInterfaccia;
    bool _CHANGEstatoInterfaccia;

    

};




class Interfaccia
{
	#define LEN_TRAMA_SCS_BUS	7

  public:
    Interfaccia(int rx, int tx);
		void begin(void);

    
  
    void Loop_Seriale();
    
    int8_t interfaccia_send_COMANDO(uint8_t A, uint8_t PL,  uint8_t stato, uint8_t attendi_risposta);
    void interfaccia_send_COMANDO_7_RAW(uint8_t* buffertx);
    void interfaccia_send_COMANDO_11_RAW(uint8_t* buffertx);
    
    void Add_Obj_Interface(abstractinterface *i){
      _interfacee[_ctn_interfacee]=i;    
      _ctn_interfacee++;
    }
    
  private:
    int _ctn_interfacee;
    abstractinterface* _interfacee[40];


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
    uint8_t Stato();

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

    void set_Timer(unsigned long timer_salita, unsigned long timer_discesa);
    void Alza(int value_percent);
    void Abbassa(int value_percent);

    void Reset_timer_flag(void);
    void action(int value_percent);

    int timer();



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

    int get_percentuale(void){
        precenttualeChange = 0;
        return stato_percentuale;
    }

    uint8_t precenttualeChange = 0;

  private:
    Interfaccia* _interfaccia;

    unsigned long TIMER_;
    int timer_flag = 0;
    int calcolo_stop_;
    unsigned long timer_salita_=4000; 
    unsigned long timer_discesa_=4000;

    int stato_percentuale=0;

    uint8_t _STATO = 0;
    unsigned long timeSTART;



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



class Serratura : public abstractinterface
{
  public:
  
    Serratura(Interfaccia* i);

    //COMANDI  
    void Sblocca();

    void address(const uint8_t A, const uint8_t PL){
      Set_Address_A(A);
    }
    void address(const uint8_t A_PL){
      Set_Address_A(A_PL);
    }

  private:
    Interfaccia* _interfaccia; 
};



class Campanello : public abstractinterface
{
  public:
  
    Campanello(Interfaccia* i);

    //COMANDI  
    uint8_t is_pressed();

    void address(const uint8_t A, const uint8_t PL){
      uint8_t add = (A * 10) + PL;
      Set_Address_A(add);
    }
    void address(const uint8_t A_PL){
      Set_Address_A(A_PL);
    }

  private:
    Interfaccia* _interfaccia; 

    unsigned long TIMERCAMPANELLO = 0;

};





















class TemperatureSensor : public abstractinterface
{
  public:

    TemperatureSensor(Interfaccia* i);

    //COMANDI  
    bool available();
    float Get();
    void Request();


    void address(const uint8_t A, const uint8_t PL){
      uint8_t add = (A * 10) + PL;
      Set_Address_A(add);
    }
    void address(const uint8_t A_PL){
      Set_Address_A(A_PL);

    }

  private:
    Interfaccia* _interfaccia;
};


























class Thermostat : public abstractinterface
{
  public:

    Thermostat(Interfaccia* i);

    //COMANDI  
    bool change();
    float Get();
    void Request_Stato();
    void Request_Temp_Setting();

    uint8_t loop();

    //invia comandi
    void set_temperature(float t);
    void set_mode(uint8_t m);



    void address(const uint8_t A, const uint8_t PL){
      uint8_t add = (A * 10) + PL;
      Set_Address_A(add);
      temperatureSensor->address(add);
      temperatureSensor->Request();
    }
    void address(const uint8_t A_PL){
      Set_Address_A(A_PL);
      temperatureSensor->address(A_PL);
      temperatureSensor->Request();
    }

  uint8_t _avaiable=0;
  float temperature;
  float temperature_di_Setting;

  private:
    Interfaccia* _interfaccia;
    TemperatureSensor* temperatureSensor;



};









	
	
#endif /* BITICINO_H_ */