void callback1(char* topic, byte* payload, unsigned int length) {
  String stringPayload;
  for (int i = 0; i < length; i++) {
    stringPayload +=(char)payload[i];
  }
  notifyClients("{\"message\":\"Mqtt nessage has arrived\"}");
  Serial.print(F("MQTT message has arrived"));
  Serial.println(stringPayload);
  StaticJsonDocument<512> GPIO;
  DeserializationError error = deserializeJson(GPIO, payload);

  // Test if parsing succeeds.
  if (error) {
     notifyClients("{\"message\":\"deserializeJson() failed:"+ String(error.f_str())+ "\"}");
    return;
  }
  notifyClients(String(stringPayload));
  strlcpy(output.mode,GPIO["mode"] ,sizeof(output.mode));
  output.red=GPIO["red"];
  output.green=GPIO["green"];
  output.blue=GPIO["blue"];
  output.brightness=GPIO["brightness"];
  if (GPIO["LED_STATUS"]==true){
    switchStripOn(output.red, output.green, output.blue, output.brightness);
    output.LED_STATUS=true;
    }
  if (GPIO["LED_STATUS"]==false){
    switchStripOff();
    output.LED_STATUS=false;
    }
  client1.publish(config.mqtt1Topic1,stringPayload.c_str());
  saveOutputFile(outputFile,stringPayload.c_str());
  }

  boolean reconnect() {
  if (client1.connect(config.mqtt1ClientId,config.mqtt1User,config.mqtt1Password)) {
  notifyClients("{\"message\":\"Connected to MQTT server\"}");  
  Serial.println(F("Connected to MQTT"));
  loadOutputFile(outputFile, output);
  printFile(outputFile); 

    client1.subscribe(config.mqtt1Topic2);
  }
  else {
    notifyClients("{\"message\":\"Can't connect to MQTT server\"}"); 
  }
  return client1.connected();
}

void broadcastState(){
  String stringPayload=getOutputStates();
  char payload[stringPayload.length()+1];
  stringPayload.toCharArray(payload,sizeof(payload)+1 );
  Serial.println(stringPayload);
  saveOutputFile(outputFile,stringPayload.c_str());
  notifyClients(stringPayload.c_str());
  client1.publish(config.mqtt1Topic1,payload);      
}
