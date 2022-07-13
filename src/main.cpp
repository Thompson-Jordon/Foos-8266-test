#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

const char *ssid = "therealslimshady";
const char *password = "pleasestandup";
char path[] = "/table/ws/test";
char host[] = "api.fuseball.org";

WebSocketsClient socket;

#define USE_SERIAL Serial

// void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
// 	const uint8_t* src = (const uint8_t*) mem;
// 	USE_SERIAL.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
// 	for(uint32_t i = 0; i < len; i++) {
// 		if(i % cols == 0) {
// 			USE_SERIAL.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
// 		}
// 		USE_SERIAL.printf("%02X ", *src);
// 		src++;
// 	}
// 	USE_SERIAL.printf("\n");
// }

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{

  USE_SERIAL.println(length);
  USE_SERIAL.println(type);

  switch (type)
  {
  case WStype_DISCONNECTED:
    USE_SERIAL.printf("[WSc] Disconnected!\n");
    break;
  case WStype_CONNECTED:
    USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);

    // send message to server when Connected
    socket.sendTXT("Connected");
    break;
  case WStype_TEXT:
    USE_SERIAL.printf("[WSc] get text: %s\n", payload);

    // send message to server
    // webSocket.sendTXT("message here");
    break;
  case WStype_BIN:
    USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
    // hexdump(payload, length);

    // send data to server
    // webSocket.sendBIN(payload, length)
    break;
  case WStype_ERROR:
  case WStype_PING:
  case WStype_PONG:
  case WStype_FRAGMENT_TEXT_START:
  case WStype_FRAGMENT_BIN_START:
  case WStype_FRAGMENT:
  case WStype_FRAGMENT_FIN:
    break;
  }
}

void setup()
{
  USE_SERIAL.begin(115200);
  USE_SERIAL.println("hello setup");
  USE_SERIAL.setDebugOutput(true);

  for (uint8_t t = 4; t > 0; t--)
  {
    USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  WiFi.begin(ssid, password);

  // WiFi.disconnect();
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
  }

  socket.begin("67.213.228.103", 8000, "/table/ws/Testing");

  // event handler
  socket.onEvent(webSocketEvent);

  // try ever 5000 again if connection has failed
  // socket.setReconnectInterval(5000);
}

void loop()
{
  delay(1000);
  USE_SERIAL.println("hello loop");
  // socket.loop();
  // put your main code here, to run repeatedly:
}