#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketClient.h>

const char *ssid = "mac&cheese";
const char *password = "42n00dl3s";
char path[] = "/table/ws/test";
char host[] = "api.fuseball.org";

void setup()
{
  Serial.begin(115200);
  Serial.println("hello setup");
}

void loop()
{
  delay(1000);
  Serial.println("hello loop");
  // put your main code here, to run repeatedly:
}