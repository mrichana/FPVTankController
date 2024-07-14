#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>

#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"

#include "OV2640.h"
#include "OV2640Streamer.h"

#include "secrets.h"

extern OV2640 camera;

#define LED 33
#define FLASH 4

#define DRLEFT 14
#define SPLEFT 2
#define DRRIGHT 13
#define SPRIGHT 15

// Web server stuff
void initWebStream(void);
void stopWebStream(void);
void handleWebServer(void);

void initWebServer(void);

void startOTA(void);
void stopOTA(void);
extern boolean otaStarted;