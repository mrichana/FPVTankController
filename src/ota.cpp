#include "main.h"

/** Flag if OTA is enabled */
boolean otaStarted = false;

void startOTA(void)
{
	ArduinoOTA.setMdnsEnabled(true);
	ArduinoOTA.setHostname(mDNS);
	ArduinoOTA.setRebootOnSuccess(true);
	// Start the OTA server
	ArduinoOTA.begin();
	otaStarted = true;
}

/**
 * Stop the OTA server
 */
void stopOTA(void)
{
	ArduinoOTA.end();
}