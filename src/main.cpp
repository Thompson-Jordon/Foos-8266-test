#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

const char *ssid = "therealslimshady";
const char *password = "pleasestandup";

WebSocketsClient socket;

#define USE_SERIAL Serial
const int DEBOUNCE_DELAY = 1000; // Debounce time in milliseconds

// Home Team Vars
int homeBeam = 4;
bool homeLastSteadyState = false;
bool homeCurrentState;
unsigned long homeLastDebounceTime = 0;

// Away Team Vars
int awayBeam = 5;
bool awayLastSteadyState = false;
bool awayCurrentState;
unsigned long awayLastDebounceTime = 0;

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{

  // USE_SERIAL.println(length);
  // USE_SERIAL.println(type);

  switch (type)
  {
  case WStype_DISCONNECTED:
    USE_SERIAL.printf("[WSc] Disconnected!\n");
    break;
  case WStype_CONNECTED:
    USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);

    // send message to server when Connected
    // socket.sendTXT("Connected");
    break;
  case WStype_TEXT:
  case WStype_BIN:
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
  pinMode(homeBeam, INPUT_PULLUP);
  pinMode(awayBeam, INPUT_PULLUP);

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

  socket.begin("67.213.228.103", 8000, "/table/ws/16fb6ae6-403e-4035-93a3-574d7ee43eab");

  // event handler
  socket.onEvent(webSocketEvent);

  // try ever 5000 again if connection has failed
  socket.setReconnectInterval(5000);
}

void checkHomeBeam()
{
  // If the switch/button changed, due to noise or pressing:
  // USE_SERIAL.println(homeCurrentState != homeLastFlickerableState);
  if ((millis() - homeLastDebounceTime) > DEBOUNCE_DELAY)
  { 
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // Get beam current state
    homeCurrentState = digitalRead(homeBeam);

    // if the button state has changed:
    if (homeLastSteadyState && !homeCurrentState)
    {
      USE_SERIAL.println("The beam is triggered");
      socket.sendTXT("ADD_HOME");
      USE_SERIAL.println("ADD_HOME");
      homeLastDebounceTime = millis();
    }
    else if (!homeLastSteadyState && homeCurrentState)
      USE_SERIAL.println("The beam is cleared");

    // save the the last steady state
    homeLastSteadyState = homeCurrentState;
  }
  return;
}

void checkAwayBeam()
{
  // Check to see if its been greater then the debounce time before checking to see if the button has been pressed
  if ((millis() - awayLastDebounceTime) > DEBOUNCE_DELAY)
  {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // Get beam current state
    awayCurrentState = digitalRead(awayBeam);

    // if the button state has changed:
    if (awayLastSteadyState && !awayCurrentState)
    {
      USE_SERIAL.println("The beam is triggered");
      socket.sendTXT("ADD_AWAY");
      USE_SERIAL.println("ADD_AWAY");
      awayLastDebounceTime = millis();
    }
    else if (!awayLastSteadyState && awayCurrentState)
      USE_SERIAL.println("The beam is cleared");

    // save the the last steady state
    awayLastSteadyState = awayCurrentState;
  }
  return;
}

void loop()
{
  checkHomeBeam();
  checkAwayBeam();

  socket.loop();
}