#include "main.h"

AsyncWebServer webserver(80);
AsyncWebSocket ws("/ws");
AsyncWebSocketClient* wsClient;

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    wsClient = client;
  } else if(type == WS_EVT_DISCONNECT){
    wsClient = nullptr;
  } else if(type == WS_EVT_DATA){
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        if (data[0] == 'L') {
       	    digitalWrite(FLASH, HIGH);
        }
    }
  }
}



static String processor(const String &var)
{
    if (var == "IPADDRESS")
        return WiFi.localIP().toString();
    return String();
}

void initWebServer(void)
{
      // Start webserver
  ws.onEvent(onWsEvent);
  webserver.addHandler(&ws);

    webserver.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                 { 
      const char* PROGMEM responsehtml = R"(
<!doctype html>
<html lang="en" class="fullscreen">
<link rel="stylesheet" href="style.css">

<head>
    <meta charset="utf-8">

    <title>Our Funky HTML Page</title>
    <meta name="description" content="Our first page">
    <meta name="keywords" content="html tutorial template">
    <meta name="viewport" content="user-scalable=no, width=device-width">
    <script>
        var requestLeft;
        var requestRight;
        var request;

        if (window.XMLHttpRequest) {
            // for modern browsers
            requestLeft = new XMLHttpRequest();
            requestRight = new XMLHttpRequest();
            request = new XMLHttpRequest();
        } else {
            // for IE6, IE5
            requestLeft = new ActiveXObject("Microsoft.XMLHTTP");
            requestRight = new ActiveXObject("Microsoft.XMLHTTP");
            request = new ActiveXObject("Microsoft.XMLHTTP");
        }

        window.addEventListener('load', onLoad);

        function initWebSocket() {
            console.log('Trying to open a WebSocket connection...');
            websocket = new WebSocket(gateway);
            websocket.onopen    = onOpen;
            websocket.onclose   = onClose;
        }

        function onOpen(event) {
            console.log('Connection opened');
        }

        function onClose(event) {
            console.log('Connection closed');
            setTimeout(initWebSocket, 2000);
        }

        function onLoad(event) {
            initWebSocket();
        }

        var gamepad;
        window.addEventListener("gamepadconnected", (event) => {
            const gamepad = event.gamepad;
            console.log("Gamepad connected:", gamepad.id);
        });
        window.addEventListener("gamepaddisconnected", (event) => {
            const gamepad = event.gamepad;
            console.log("Gamepad disconnected:", gamepad.id);
        });


        function left(speed){
            console.log('L'+speed)
            // requestLeft.open('GET', "/lefttrack?speed="+(-speed));
            // requestLeft.send(null);
            websocket.send("l"+(-speed));
        }
        function right(speed){
            console.log('R'+speed)
            // requestLeft.open('GET', "/righttrack?speed="+(-speed));
            // requestLeft.send(null);
            websocket.send("r"+(-speed));
        }
        var left=0;
        var right=0;
        var button0=false;
        var button1=false;
        function handleGamepadInput() {
            const gamepads = navigator.getGamepads();
            for (const gamepad of gamepads) {
                if (gamepad) {
                    var leftaxis = +gamepad.axes[1].toFixed(2);
                    if (leftaxis != left) {
                        left=leftaxis;
                        console.log(leftaxis)
                        // requestLeft.open('GET', "/lefttrack?speed="+(-leftaxis));
                        // requestLeft.send(null);
                        websocket.send("l"+(-leftaxis));
                    }


                    var rightaxis = +gamepad.axes[3].toFixed(2);
                    if (rightaxis != right) {
                        right=rightaxis;
                        console.log(rightaxis)
                        requestLeft.open('GET', "/righttrack?speed="+(-rightaxis));
                        requestLeft.send(null);

                    }

                    var b0 = gamepad.buttons[0];
                    if (!button0 && b0) {
                        console.log("Button 1 press");
                        button0 = true;
                        request.open('GET', "/ledon");
                        request.send(null);
                    }
                    if (button0 && !b0) {
                        console.log("Button 1 release");
                        button0 = false;
                    }

                    var b1 = gamepad.buttons[1];
                    if (!button1 && b1) {
                        console.log("Button 1 press");
                        button1 = true;
                        request.open('GET', "/ledoff");
                        request.send(null);

                    }
                    if (button1 && !b1) {
                        console.log("Button 1 release");
                        button1 = false;
                    }

               }
            }
        }
        setInterval(handleGamepadInput, 200);
    </script>
</head>

<body class="fullscreen" onclick=>
    <img  class="fullscreen" alt="Camera Feed" src="http://%IPADDRESS%:81/">
</body>

</html>
)";


    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", responsehtml, processor);
    request->send(response); });

    webserver.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
                 { 
      const char* PROGMEM responsecss = R"(
        .fullscreen {
            height: 100%;
        }
    )";

    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", responsecss);
    request->send(response); });

    webserver.on("/ledon", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
	    digitalWrite(FLASH, HIGH);
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", "Flash On");
         request->send(response); });

    webserver.on("/ledoff", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
	    digitalWrite(FLASH, LOW);
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", "Flash Off");
         request->send(response); });

    webserver.on("/lefttrack", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
            digitalWrite(DRLEFT, false);
            analogWrite(SPLEFT, false);
        
        float value;
            if (request->hasParam("speed")) {
            value = request->getParam("speed")->value().toFloat();
        } else {
            value = 1.0F;
        }
        unsigned short speed;
        if (value > 0) {
            speed = value * 255;
            digitalWrite(DRLEFT, true);
            analogWrite(SPLEFT, 255-speed);
        } else {
            speed = (-value) * 255;
            digitalWrite(DRLEFT, false);
            analogWrite(SPLEFT, speed);
        }     
        String ret = String("Left:");
        ret += value>=0?"":"-";
        ret += speed;
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", ret.c_str());
        request->send(response); });

    webserver.on("/righttrack", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
        float value;
            if (request->hasParam("speed")) {
            value = request->getParam("speed")->value().toFloat();
        } else {
            value = 1.0F;
        }
        unsigned short speed;
        if (value > 0) {
            speed = value * 255;
            digitalWrite(DRRIGHT, true);
            analogWrite(SPRIGHT, 255-speed);
        } else {
            speed = (-value) * 255;
            digitalWrite(DRRIGHT, false);
            analogWrite(SPRIGHT, speed);
        }     
        String ret = String("Right:");
        ret += value>=0?"":"-";
        ret += speed;
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", ret.c_str());
        request->send(response); });

    webserver.on("/stop", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
        digitalWrite(DRLEFT, false);
        analogWrite(SPLEFT, 0);
        digitalWrite(DRRIGHT, false);
        analogWrite(SPRIGHT, 0);
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", "Stop");
        request->send(response); });

    webserver.begin();
};
