WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
WiFiClientSecure espClient;

extern void ACCENDI(String deviceId);
extern void SPEGNI(String deviceId);
extern void TERMOSTATO_IMPOSTA_TEMPERATURA(String deviceId, float target_temp, char mode);
extern void TERMOSTATO_IMPOSTA_MODO(String deviceId, String t_mode);
extern void DIMMER_CHANGE_PERCENT(String deviceId, uint8_t percent);


extern Interfaccia interfaccia;                            //D6"ESP"=TX INTERFACCIA SCS "CON INTERFACCIA NUOVA"

PubSubClient client(AWS_endpoint, 8883, callback, espClient); //set MQTT port number to 8883 as per //standard

void callback(char* topic, byte* payload, unsigned int length) {
  /*  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  */

  //https://arduinojson.org/v6/doc/upgrade/
  DynamicJsonDocument jsonBuffer(512);         
  auto error = deserializeJson(jsonBuffer, (char*)payload);
  if (error) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
    return;
  }
  
  String deviceTipo = jsonBuffer["tipo"];     
  String deviceStato = jsonBuffer["stato"];
  String deviceID = jsonBuffer["identificativo"];

 
  
  if(deviceTipo == "Alexa.PowerController") {                                       // Switch or Light
    if(deviceStato == "TurnOn") { 
      ACCENDI(deviceID);
    }else if(deviceStato == "TurnOff"){
      SPEGNI(deviceID);      
    }   
  }else if(deviceTipo == "Alexa.BrightnessController"){                       //DIMMER
    if(deviceStato == "SetBrightness") { 
      String p = jsonBuffer["p"];
      uint8_t vp = p.toInt();
      DIMMER_CHANGE_PERCENT(deviceID,vp);
    }else if(deviceStato == ""){
    }       
  }else if(deviceTipo == "Alexa.ThermostatController"){                             //TERMOSTATO
    if(deviceStato == "mode") { 
      String mod = jsonBuffer["m"];
      TERMOSTATO_IMPOSTA_MODO(deviceID, mod);
    }else if(deviceStato == "temp") { 
      float t = jsonBuffer["t"];
      TERMOSTATO_IMPOSTA_TEMPERATURA(deviceID,t, 't');
    }
  }else if(deviceTipo == "Alexa.RangeController"){                                  //SERRANDE 
    int v = deviceStato.toInt();
    ALEXA_SERRANDA(deviceID, v);
  }else if(deviceTipo == "Alexa.LockController"){                                   //SERRATURA 
    if(deviceStato == "Unlock" ){
     SERRATURA_SBLOCCA(deviceID); 
    }
  }
  
  //x altri dispositivi
  
  

}


void reconnect(String ssubscribe) {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {       
    //if (client.connect("ESPthing")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe(ssubscribe.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
    char buf[256];
    espClient.getLastSSLError(buf,256);
    Serial.print("WiFiClientSecure SSL error: ");
    Serial.println(buf);

    // Wait 5 seconds before retrying
    delay(5000);
    }
  }
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  espClient.setBufferSizes(512, 512);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  timeClient.begin();
  while(!timeClient.update()){
    timeClient.forceUpdate();
  }
  
  espClient.setX509Time(timeClient.getEpochTime());
}


void client_mqtt_loop(String ssubscribe){
  if (!client.connected()) {
    reconnect(ssubscribe);
  }
  client.loop();
}


void setup_certificate(void){
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }

  // Load certificate file
  File cert = SPIFFS.open("/cert.der", "r"); //replace cert.crt eith your uploaded file name
  if (!cert) {
    Serial.println("Failed to open cert file");
  }
  else
    Serial.println("Success to open cert file");
    
  delay(1000);

  if (espClient.loadCertificate(cert))
    Serial.println("cert loaded");
  else
    Serial.println("cert not loaded");

  // Load private key file
  File private_key = SPIFFS.open("/private.der", "r"); //replace private eith your uploaded file name
  if (!private_key) {
    Serial.println("Failed to open private cert file");
  }
  else
    Serial.println("Success to open private cert file");

  delay(1000);

  if (espClient.loadPrivateKey(private_key))
    Serial.println("private key loaded");
  else
    Serial.println("private key not loaded");

  // Load CA file
  File ca = SPIFFS.open("/ca.der", "r"); //replace ca eith your uploaded file name
  if (!ca) {
    Serial.println("Failed to open ca ");
  }
  else
    Serial.println("Success to open ca");

  delay(1000);

  if(espClient.loadCACert(ca))
    Serial.println("ca loaded");
  else
    Serial.println("ca failed");

}

//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

void send_state_to_Alexa_synchronous(String deviceId , String tipo, String stato){
  if (client.connected()) {
	  DynamicJsonDocument root(256);
	  char output[256];

	  root["id"] = deviceId;
	  root["stato"] = stato;
	  root["tipo"] = tipo;
	  //root.printTo(databuf);
	  serializeJson(root, output);
	  client.publish("ESP8266/in", output);
	}
}



void send_Event_Campanello(void){
  DynamicJsonDocument root(150);
  char output[150];

  root["id"] = "doorbell-01";
  root["stato"] = "PHYSICAL_INTERACTION";
  //root.printTo(databuf);
  serializeJson(root, output);
  client.publish("ESP8266/out/event", output);
}
