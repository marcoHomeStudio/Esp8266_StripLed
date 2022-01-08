void notifyClients(String state) {
  ws.textAll(state);
}
void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    
    if (strcmp((char*)data, "states") == 0) {
      Serial.print("outputState=");
      Serial.println(getOutputStates());
      notifyClients(getOutputStates());
      String stringPayload=getOutputStates();
      char payload[stringPayload.length()+1];
      stringPayload.toCharArray(payload,sizeof(payload)+1 );
      saveOutputFile(outputFile,stringPayload.c_str());
      client1.publish(config.mqtt1Topic1,payload);      
    }
    else {

      StaticJsonDocument<512> fromClient;
      DeserializationError error = deserializeJson(fromClient, (char*)data);
    
      // Test if parsing succeeds.
      if (error) {
         notifyClients("{\"message\":\"deserializeJson() failed:"+ String(error.f_str())+ "\"}");
        return;
      }
      strlcpy(output.mode,fromClient["mode"] ,sizeof(output.mode));
      output.red=fromClient["red"];
      output.green=fromClient["green"];
      output.blue=fromClient["blue"];
      output.brightness=fromClient["brightness"];
      if (fromClient["LED_STATUS"]==true){
        switchStripOn(output.red, output.green, output.blue, output.brightness);
        output.LED_STATUS=true;
        }
      if (fromClient["LED_STATUS"]==false){
        switchStripOff();
        output.LED_STATUS=false;
        }
      String jsonString;
      serializeJson(fromClient, jsonString);
      Serial.print("From client:");
      Serial.println(jsonString);
      notifyClients(jsonString.c_str());
      client1.publish(config.mqtt1Topic1,jsonString.c_str());
      saveOutputFile(outputFile,jsonString.c_str());
      }
    }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
               void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}
