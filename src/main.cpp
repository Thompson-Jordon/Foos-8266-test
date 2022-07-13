#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

const char *ssid = "therealslimshady";
const char *password = "pleasestandup";

WebSocketsClient socket;

#define USE_SERIAL Serial
const int DEBOUNCE_TIME = 1000; // Debounce time in milliseconds
struct Beam
{
  int PIN;
  bool currentState = false;
  int lastPushTime = 0;
};

Beam homeBeam = {16, false, millis()}; // Pin connected to home side break beam
Beam awayBeam = {5, false, millis()}; // Pin connected to away side break beam

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

bool debounceBeam(Beam * beam)
{
  int now = millis();
  int timeGap = now - beam->lastPushTime;

  bool state = digitalRead(beam->PIN);

  // USE_SERIAL.println(timeGap > DEBOUNCE_TIME);

  if (timeGap > DEBOUNCE_TIME)
  {
    if (!state)
    {
      beam->currentState = true;
    }
    else
    {
      beam->currentState = false;
    }
  }

  beam->lastPushTime = now;
  return beam->currentState;
}

void setup()
{
  pinMode(homeBeam.PIN, INPUT_PULLUP);
  pinMode(awayBeam.PIN, INPUT_PULLUP);

  USE_SERIAL.begin(115200);

  USE_SERIAL.println("Start Setup");

  for (uint8_t t = 4; t > 0; t--)
  {
    USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }
  WiFi.mode(WIFI_AP);

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
  socket.setReconnectInterval(5000);
}

void loop()
{
  homeBeam.currentState = debounceBeam(&homeBeam);
  awayBeam.currentState = debounceBeam(&awayBeam);


  if (homeBeam.currentState) socket.sendTXT("ADD_HOME");
  if (awayBeam.currentState) socket.sendTXT("ADD_AWAY");
  socket.loop();
}