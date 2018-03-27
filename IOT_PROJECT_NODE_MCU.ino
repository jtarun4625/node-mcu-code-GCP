
/*
 * WebSocketClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>

#include <dht.h>

#include <ESP8266WiFi.h>

#include <WebSocketsClient.h>

#include <ArduinoJson.h>

#include <Hash.h>

WebSocketsClient webSocket;

#define USE_SERIAL Serial



//define mux pins
#define sig A0      // Analog In Pin 0


const char* ssid     = "dlink";
const char* password = "ninjafx4";
dht DHT;

#define DHT11_PIN D7
int LedPin = D3;
boolean check = false;
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

	switch(type) {
		case WStype_DISCONNECTED:
			USE_SERIAL.printf("[WSc] Disconnected!\n");
			break;
		case WStype_CONNECTED: {
			USE_SERIAL.printf("%s\n", payload);
      check = parse_payload_for_fan(payload);
     fan(check);
//      parse_payload_for_fan(payload);

			// send message to server when Connected
//			webSocket.sendTXT("Connected");
		}
			break;
		case WStype_TEXT:
			USE_SERIAL.printf("%s\n", payload);
     check = parse_payload_for_fan(payload);
      if(check){
          fan(false);
        }else{
          fan(true);
          }

     delay(5000);

			// send message to server
			// webSocket.sendTXT("message here");
			break;
		case WStype_BIN:
			USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
			hexdump(payload, length);

			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
	}

}

void setup() {
	// USE_SERIAL.begin(921600);
	USE_SERIAL.begin(115200);
  delay(10);

  pinMode(LedPin, OUTPUT);
  fan(false);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
//
//	//Serial.setDebugOutput(true);
//	USE_SERIAL.setDebugOutput(true);
//
//	USE_SERIAL.println();
//	USE_SERIAL.println();
//	USE_SERIAL.println();
//
//	for(uint8_t t = 4; t > 0; t--) {
//		USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
//		USE_SERIAL.flush();
//		delay(1000);
//	}

  WiFi.begin(ssid, password);


 while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n");
	// server address, port and URL
  Serial.println("Connecting Websocket \n ");
	webSocket.begin("thetechnow.in", 80, "/");

	// event handler
	webSocket.onEvent(webSocketEvent);

	// use HTTP Basic Authorization this is optional remove if not needed
//	webSocket.setAuthorization("user", "Password");

	// try ever 5000 again if connection has failed
	webSocket.setReconnectInterval(5000);
  digitalWrite(LedPin, LOW);   // turn the LED on (HIGH is the voltage level)

}

void loop() {
  int water_sensor = analogRead(sig);
  Serial.println();      // NEW LINE
//  Serial.println(water_sensor);

  Serial.println();
	webSocket.loop();
//   digitalWrite(LedPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  check_temp_humidity();
//     digitalWrite(LedPin, LOW);   // turn the LED on (HIGH is the voltage level)
//  check_temp_humidity();

}


void check_temp_humidity(){
    int chk = DHT.read11(DHT11_PIN);
    int temp_out = ((float)DHT.temperature);
    int humidity = ((float)DHT.humidity);
    Serial.print("\n");
    Serial.print("\n");
    String test = "{\"temp\": ";
    test+=temp_out;
    test+= ",\"humid\":";
    test+=humidity;
    test+="}";
    Serial.print(test);
    webSocket.sendTXT(test);
    Serial.print(check);
    delay(1000);
  }
bool parse_payload_for_fan(uint8_t * payload){
    StaticJsonBuffer<200> jsonBuffer;
//    char json[] = (char*) payload;

  // Step 4
  JsonObject& root = jsonBuffer.parseObject(payload);

  // Step 5
  if (!root.success()) {
    Serial.println("parseObject() failed");
  }

  // Step 6
  boolean fan = root["fan"];
  
  Serial.println(fan);

  return fan;
  }

void fan(boolean fan){
    if(fan){
      Serial.println("Fan ONN");
         digitalWrite(LedPin, HIGH);   // turn the LED on (HIGH is the voltage level)
}else{
        Serial.println("Fan Off");

     digitalWrite(LedPin, LOW);  
}
  }


