void loadOutputFile(const char *outputFile, Output &output){
    File file2 = LittleFS.open(outputFile, "r");
  StaticJsonDocument<1024> stripled;
  DeserializationError error = deserializeJson(stripled, file2);
  if (error)
    Serial.println(F("Failed to read file, using default output file"));
  output.LED_STATUS = stripled["LED_STATUS"] | false;
  strlcpy(output.mode,                  // <- destination
          stripled["mode"] | "light",  // <- source
          sizeof(output.mode));         // <- destination's capacity
  
  output.red=stripled["red"] | 255;
  output.green=stripled["green"] | 255;
  output.blue=stripled["blue"] | 255;
  output.brightness=stripled["brightness"] | 255; 
  file2.close();
  if (output.LED_STATUS==true){
    switchStripOn(output.red, output.green, output.blue, output.brightness);
  }
  if (output.LED_STATUS==false){
    switchStripOff();
  }
  
  String jsonString;
  serializeJson(stripled, jsonString);
  Serial.print("Output file:");
  Serial.println(jsonString);
  client1.publish(config.mqtt1Topic1,jsonString.c_str());
  
}

void loadConfiguration(const char *configFile, Config &config) {
  File file = LittleFS.open(configFile, "r");
  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));
  config.init = doc["init"] | false;
  strlcpy(config.APname,                  // <- destination
          doc["APname"] | "Esp8266sensor",  // <- source
          sizeof(config.APname));         // <- destination's capacity
  strlcpy(config.APkey,
          doc["APkey"] | "123456789",
          sizeof(config.APkey));
  strlcpy(config.ESPhostname,
          doc["ESPhostname"] | "ESP4output",
          sizeof(config.ESPhostname));
  strlcpy(config.friendlyName,
          doc["friendlyName"] | "ESP8266 4 output module",
          sizeof(config.friendlyName));
  strlcpy(config.SSID,
          doc["SSID"] | "myApKey",
          sizeof(config.SSID));
  strlcpy(config.wifikey,
          doc["wifikey"] | "myWifiKey",
          sizeof(config.wifikey));
  strlcpy(config.username,
          doc["username"] | "admin",
          sizeof(config.username));
  strlcpy(config.password,
          doc["password"] | "admin",
          sizeof(config.password));
  strlcpy(config.mqtt1Server,
          doc["mqtt1Server"] | "pidev",
          sizeof(config.mqtt1Server));
  config.mqtt1Port=doc["mqtt1Port"] | 1883;
  strlcpy(config.mqtt1User,
          doc["mqtt1User"] | "admin",
          sizeof(config.mqtt1User));
  strlcpy(config.mqtt1Password,
          doc["mqtt1Password"] | "admin",
          sizeof(config.mqtt1Password));         
  strlcpy(config.mqtt1ClientId,
          doc["mqtt1ClientId"] | "DustSensor",
          sizeof(config.mqtt1ClientId));
  strlcpy(config.mqtt1Topic1,
          doc["mqtt1Topic1"] | "Esp8266Sensor/dustDensity",
          sizeof(config.mqtt1Topic1));
  strlcpy(config.mqtt1Topic2,
          doc["mqtt1Topic2"] | "Esp8266Sensor/stdDeviation",
          sizeof(config.mqtt1Topic2));
  // Close the file (Curiously, File's destructor doesn't close the file)
  file.close();
}

void saveOutputFile(const char *outputFile, const char *payload){
  LittleFS.remove(outputFile);
  File file = LittleFS.open(outputFile, "w");
  if (!file) {
    Serial.println(F("Failed to create file"));
    return;
  }
  file.print((char*)payload);
  delay(1);
  //Close the file
  file.close();

}

void saveConfiguration(const char *configFile, const Config &config) {
  // Delete existing file, otherwise the configuration is appended to the file
  LittleFS.remove(configFile);
  // Open file for writing
  File file = LittleFS.open(configFile, "w");
  if (!file) {
    Serial.println(F("Failed to create file"));
    return;
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<1024> doc;
  // Set the values in the document
  doc["init"] = config.init;
  doc["APname"]=config.APname;
  doc["APkey"]=config.APkey;
  doc["ESPhostname"] = config.ESPhostname;
  doc["friendlyName"] = config.friendlyName;
  doc["SSID"] = config.SSID;
  doc["wifikey"] = config.wifikey;
  doc["username"] = config.username;
  doc["password"] = config.password;
  doc["mqtt1Server"] = config.mqtt1Server;
  doc["mqtt1Port"] = config.mqtt1Port;
  doc["mqtt1Password"] = config.mqtt1Password;
  doc["mqtt1User"] = config.mqtt1User;
  doc["mqtt1ClientId"] = config.mqtt1ClientId;
  doc["mqtt1Topic1"] = config.mqtt1Topic1;
  doc["mqtt1Topic2"] = config.mqtt1Topic2;
  // Serialize JSON to file
  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }

  // Close the file
  file.close();
}

void resetConfig(){
  config.init=true;
  strlcpy(config.password,  "12345", sizeof(config.password));
  strlcpy(config.username,  "admin", sizeof(config.username));
  strlcpy(config.SSID,  "<YOUR SSID>", sizeof(config.SSID));
  strlcpy(config.mqtt1Server,  "<YOUR MQTT SERVER>", sizeof(config.mqtt1Server));
  config.mqtt1Port=1883;
  strlcpy(config.mqtt1User,  "<YOUR MQTT USER>", sizeof(config.mqtt1User));
  strlcpy(config.mqtt1Password,  "<YOUR MQTT PASSWORD>", sizeof(config.mqtt1Password));
  strlcpy(config.mqtt1ClientId,  "<YOUR MQTT CLIENTID>", sizeof(config.mqtt1ClientId));
  strlcpy(config.mqtt1Topic1,  "<YOUR MQTT PUBLISH CHANNEL>", sizeof(config.mqtt1Topic1));
  strlcpy(config.mqtt1Topic2,  "<YOUR MQTT SUBSCRIBE CHANNEL>", sizeof(config.mqtt1Topic2)); 
  strlcpy(config.wifikey,  "<YOUR WIFI KEY>", sizeof(config.wifikey)); 
  strlcpy(config.ESPhostname,  "<DEVICE HOSTNAME>", sizeof(config.ESPhostname)); 
      
  saveConfiguration(configFile, config);
}

void printFile(const char *configFile) {
  // Open file for reading
  File file = LittleFS.open(configFile, "r");
  if (!file) {
    Serial.println(F("Failed to read file"));
    return;
  }

  // Extract each characters by one by one
  while (file.available()) {
    Serial.print((char)file.read());
  }
  Serial.println();

  // Close the file
  file.close();
}
