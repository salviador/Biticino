
#define HEARTBEAT_INTERVAL 300000 // 5 Minutes 
uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
WiFiClient client;

//void setPowerStateOnServer(String deviceId, String value);

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      isConnected = false;    
      Serial.printf("[WSc] Webservice disconnected from sinric.com!\n");
      break;
    case WStype_CONNECTED: {
      isConnected = true;
      Serial.printf("[WSc] Service connected to sinric.com at url: %s\n", payload);
      Serial.printf("Waiting for commands from sinric.com ...\n");        
      }
      break;
    case WStype_TEXT: {
        Serial.printf("[WSc] get text: %s\n", payload);
        // Example payloads

        // For Switch or Light device types
        // {"deviceId": xxxx, "action": "setPowerState", value: "ON"} // https://developer.amazon.com/docs/device-apis/alexa-powercontroller.html

        // For Light device type
        // Look at the light example in github


        //https://arduinojson.org/v6/doc/upgrade/
        DynamicJsonDocument jsonBuffer(255);         
        auto error = deserializeJson(jsonBuffer, (char*)payload);
        if (error) {
          Serial.print(F("deserializeJson() failed with code "));
          Serial.println(error.c_str());
          break;
        }
        String deviceId = jsonBuffer["deviceId"];     
        String action = jsonBuffer["action"];

        
        //DynamicJsonBuffer jsonBuffer;
        //JsonObject& json = jsonBuffer.parseObject((char*)payload); 
        //String deviceId = json ["deviceId"];     
        //String action = json ["action"];
        
        if(action == "setPowerState") { // Switch or Light
            //String value = json ["value"];
            String value = jsonBuffer ["value"];
            if(value == "ON") {
                ACCENDI(deviceId);
            } else {
                SPEGNI(deviceId);
            }
        }else if(action == "AdjustPercentage") { 
           // Alexa, alza serranda
           // Alexa, abbassa serranda           
           // Alexa, ferma la serranda           

           String value = jsonBuffer["value"]["percentageDelta"];
           Serial.println("[WSc] value: " + value); 

           int percentuale = value.toInt();

            if(percentuale > 0){              
              ALZA(deviceId, percentuale);  
            } else if(percentuale < 0){              
              ABBASSA(deviceId, percentuale);  
            }

           // if value: 1 == UP
           // if value: -1 == Down
        } else if (action == "SetPercentage") {

           String value = jsonBuffer["value"]["percentage"];
           Serial.println("[WSc] value: " + value); 

           int percentuale = value.toInt();

            if (percentuale > 50) {
              ALZA(deviceId, percentuale);  
            } else if (percentuale < 50) {
              ABBASSA(deviceId, percentuale);  
            }
          
        } else if (action == "test") {
            Serial.println("[WSc] received test command from sinric.com");
        }
      }
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      break;
  }
 }

void sinric_connetti(void){
  WiFiMulti.addAP(MySSID, MyWifiPassword);
  Serial.println();
  Serial.print("Connecting to Wifi: ");
  Serial.println(MySSID);  

  // Waiting for Wifi connect
  while(WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if(WiFiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.print("WiFi connected. ");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  // server address, port and URL
  webSocket.begin("iot.sinric.com", 80, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);
  webSocket.setAuthorization("apikey", MyApiKey);
  
  // try again every 5000ms if connection has failed
  webSocket.setReconnectInterval(5000);   // If you see 'class WebSocketsClient' has no member named 'setReconnectInterval' error update arduinoWebSockets


}

void sinric_loop(void){
  webSocket.loop();
  
  if(isConnected) {
      uint64_t now = millis();
      
      // Send heartbeat in order to avoid disconnections during ISP resetting IPs over night. Thanks @MacSass
      if((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
          heartbeatTimestamp = now;
          webSocket.sendTXT("H");          
      }
  } 
}

// If you are going to use a push button to on/off the switch manually, use this function to update the status on the server
// so it will reflect on Alexa app.
// eg: setPowerStateOnServer("deviceid", "ON")

// Call ONLY If status changed. DO NOT CALL THIS IN loop() and overload the server. 
void setPowerStateOnServer(String deviceId, String value) {
  DynamicJsonDocument root(150);
  //DynamicJsonBuffer jsonBuffer;
  //JsonObject& root = jsonBuffer.createObject();
  root["deviceId"] = deviceId;
  root["action"] = "setPowerState";
  root["value"] = value;
  StreamString databuf;
  //root.printTo(databuf);
  serializeJson(root, databuf);
  
  webSocket.sendTXT(databuf);
}
