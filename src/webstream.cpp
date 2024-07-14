#include "main.h"

/** Web server class */
WebServer server(81);

/** Forward dedclaration of the task handling browser requests */
void webTask(void *pvParameters);

/** Task handle of the web task */
TaskHandle_t webTaskHandler;

/** Flag to stop the web server */
volatile boolean stopWeb = false;

/** Web request function forward declarations */
void handle_jpg_stream(void);
void handle_ledon(void);
void handle_ledoff(void);
void handle_start_ota(void);
void handleNotFound();

/**
 * Initialize the web stream server by starting the handler task
 */
void initWebStream(void)
{
	// Create the task for the web server and run it in core 0 (others tasks use core 1)
	xTaskCreatePinnedToCore(webTask, "WEB", 4096, NULL, 1, &webTaskHandler, 0);

	if (webTaskHandler == NULL)
	{
		Serial.println("Create Webstream task failed");
	}
	else
	{
		Serial.println("Webstream task up and running");
	}
}

/**
 * Called to stop the web server task, needed for OTA
 * to avoid OTA timeout error
 */
void stopWebStream(void)
{
	stopWeb = true;
}

/**
 * The task that handles web server connections
 * Starts the web server
 * Handles requests in an endless loop
 * until a stop request is received because OTA
 * starts
 */
void webTask(void *pvParameters)
{
	// Set the function to handle stream requests
	server.on("/", HTTP_GET, handle_jpg_stream);
	// Set the function to handle single picture requests
	server.on("/ota", HTTP_GET, handle_start_ota);
	// Set the function to handle other requests
	server.onNotFound(handleNotFound);
	// Start the web server
	server.begin();

	while (1)
	{

		// Check if the server has clients
		server.handleClient();
	}
	if (stopWeb)
	{
		// User requested web server stop
		server.close();
		// Delete this task
		vTaskDelete(NULL);
	}
	delay(100);
}

/**
 * Handle web stream requests
 * Gives a first response to prepare the streaming
 * Then runs in a loop to update the web content
 * every time a new frame is available
 */
void handle_jpg_stream(void)
{
	WiFiClient thisClient = server.client();
	String response = "HTTP/1.1 200 OK\r\n";
	response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
	server.sendContent(response);

	while (1)
	{
		camera.run();
		if (!thisClient.connected())
		{
			break;
		}
		response = "--frame\r\n";
		response += "Content-Type: image/jpeg\r\n\r\n";
		server.sendContent(response);

		thisClient.write((char *)camera.getfb(), camera.getSize());
		server.sendContent("\r\n");
		delay(20);
	}
}

void handle_start_ota(void)
{
	IPAddress ip = WiFi.localIP();
	String message = "Going into OTA mode";
	message += "\n";
	server.send(200, "text/plain", message);
	stopWebStream();
	delay(500);
	startOTA();
}

/**
 * Handle any other request from the web client
 */
void handleNotFound()
{
}
