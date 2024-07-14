#include "main.h"

OV2640 camera;
u_int8_t flash = 33;
u_int8_t led = 4;

void blink(uint8_t);


void setup()
{
  Serial.begin(115200);
  WiFi.setHostname(mDNS);
  WiFi.begin(SSID, PASWD);

  pinMode(DRLEFT, OUTPUT);
  pinMode(SPLEFT, OUTPUT);
  pinMode(DRRIGHT, OUTPUT);
  pinMode(SPRIGHT, OUTPUT);

  pinMode(LED, OUTPUT); // Red Led
  digitalWrite(LED, LOW);

  pinMode(FLASH, OUTPUT);
  digitalWrite(FLASH, LOW);

  // camera.init(esp32cam_aithinker_config);
  // sensor_t *s = esp_camera_sensor_get();
  // s->set_vflip(s, 1);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
  }

  MDNS.begin(mDNS);
  // initWebStream();
  initWebServer();
}

unsigned long previousMillis;
void loop()
{
  if (otaStarted)
  {
    digitalWrite(DRLEFT, LOW);
    analogWrite(SPLEFT, LOW);
    digitalWrite(DRRIGHT, LOW);
    analogWrite(SPRIGHT, LOW);
    
    ArduinoOTA.handle();
    digitalWrite(LED, LOW);
  }
  else
  {
    blink(LED);

    unsigned long currentMillis = millis();
    // if WiFi is down, try reconnecting
    if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= 300)) {
      Serial.print(millis());
      Serial.println("Reconnecting to WiFi...");
      while (!WiFi.reconnect()) {};
      previousMillis = currentMillis;
    }
  }
}

void blink(uint8_t led)
{
  static unsigned long lasttime = 0;
  if (millis() - lasttime > 1000)
  {
    lasttime = millis();
    digitalWrite(led, !digitalRead(led));
  }
}

