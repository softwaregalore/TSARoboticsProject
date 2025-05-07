//include our UI and necessary server libraries
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "index.h"

//network ssid and password
const char *ssid = "OwenRobotControl";
const char *password = "TSATSATSA";

//Port 80 with websockets
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

//IP address and configuration information
IPAddress local_IP(200,0,0,1);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);


//IO expander
#include <Adafruit_MCP23X17.h>
Adafruit_MCP23X17 mcp;
//define our three servos
#include <ESP32Servo.h>
Servo pickup;
Servo tilt;
Servo balance;
//all the pins for our robot
int PickupPin = 13;
int TiltPin = 12;
int BalancePin = 14;

int BalanceBackSwitch = 14;
int BalanceFrontSwitch = 15;

int MotorPWM1 = 15;
int MotorA1 = 0;
int MotorB1 = 1;
int Lift1DownSwitch = 27;
int Lift1UpSwitch = 26;

int MotorPWM2 = 2;
int MotorA2 = 2;
int MotorB2 = 3;
int Lift2DownSwitch = 25;
int Lift2UpSwitch = 33;

int MotorPWM3 = 4;
int MotorA3 = 4;
int MotorB3 = 5;
int Lift3DownSwitch = 32;
int Lift3UpSwitch = 35;

int MotorPWM4 = 5;
int MotorA4 = 6;
int MotorB4 = 7;

int MotorPWM5 = 18;
int MotorA5 = 8;
int MotorB5 = 9;

int MotorPWM6 = 19;
int MotorA6 = 10;
int MotorB6 = 11;

int MotorPWM7 = 23;
int MotorA7 = 12;
int MotorB7 = 13;

//speed and directions for everything
int Motor1Direction = 1;
int Motor1Speed = 0;
int Motor2Direction = 1;
int Motor2Speed = 0;
int Motor3Direction = 1;
int Motor3Speed = 0;
int Motor4Direction = 1;
int Motor4Speed = 0;
int Motor5Direction = 1;
int Motor5Speed = 0;
int Motor6Direction = 1;
int Motor6Speed = 0;
int Motor7Direction = 1;
int Motor7Speed = 0;
//servo angles
int PickupAngle = 90;
int TiltAngle = 90;
int BalanceAngle = 90;

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    //when we receive data, decode (really just split up) our info and set the appropriate motor directions and speeds.
    String Data;
    for (int i = 0; i < len; i += 1) {
      Data += char(data[i]);
      switch (i) {
        case 0:
          Motor1Direction = Data.toInt();
          Data = "";
          break;
        case 3:
          Motor1Speed = Data.toInt();
          Data = "";
          break;
        case 4:
          Motor2Direction = Data.toInt();
          Data = "";
          break;
        case 7:
          Motor2Speed = Data.toInt();
          Data = "";
          break;
        case 8:
          Motor3Direction = Data.toInt();
          Data = "";
          break;
        case 11:
          Motor3Speed = Data.toInt();
          Data = "";
          break;
        case 12:
          Motor4Direction = Data.toInt();
          Data = "";
          break;
        case 15:
          Motor4Speed = Data.toInt();
          Data = "";
          break;
        case 16:
          Motor5Direction = Data.toInt();
          Data = "";
          break;
        case 19:
          Motor5Speed = Data.toInt();
          Data = "";
          break;
        case 20:
          Motor6Direction = Data.toInt();
          Data = "";
          break;
        case 23:
          Motor6Speed = Data.toInt();
          Data = "";
          break;
        case 24:
          Motor7Direction = Data.toInt();
          Data = "";
          break;
        case 27:
          Motor7Speed = Data.toInt();
          Data = "";
          break;
        case 30:
          PickupAngle = Data.toInt();
          Data = "";
          break;
        case 33:
          TiltAngle = Data.toInt();
          Data = "";
          break;
        case 36:
          BalanceAngle = Data.toInt();
          Data = "";
          break;
      }
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
    switch (type) {
      case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
      case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        //if our robot disconnects for whatever reason, we do not want it to just keep on moving. (we definitely didn't learn this from experience)
        Motor1Direction = 1;
        Motor1Speed = 0;
        Motor2Direction = 1;
        Motor2Speed = 0;
        Motor3Direction = 1;
        Motor3Speed = 0;
        Motor4Direction = 1;
        Motor4Speed = 0;
        Motor5Direction = 1;
        Motor5Speed = 0;
        Motor6Direction = 1;
        Motor6Speed = 0;
        Motor7Direction = 1;
        Motor7Speed = 0;

        break;
      case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
      case WS_EVT_PONG:
      case WS_EVT_ERROR:
        break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}
String processor(const String& var){
  Serial.println(var);
  return String();
}

void setup() {
  //setting up our debugging system
  Serial.begin(9600);
  Serial.println("Testing Testing 123");
  //setting up wifi control
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssid, password);
  //initializing the websocket
  initWebSocket();
  //define what data our server will send, in this case our html file included earlier
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", WEBpage, processor);
  });
  server.begin();

  //setup or i2c io expander
  //for future reference, to use a pin on the expander, just begin with an "mcp.". Same otherwise.
  mcp.begin_I2C(0x27);

  //setting up the servos
  pickup.attach(PickupPin);
  tilt.attach(TiltPin);
  balance.attach(BalancePin);
  //defining our many pins
  //"mcp" means that the pin is on our IO expander board
  pinMode(MotorPWM1, OUTPUT);
  mcp.pinMode(MotorA1, OUTPUT);
  mcp.pinMode(MotorB1, OUTPUT);
  pinMode(Lift1DownSwitch, INPUT);
  pinMode(Lift1UpSwitch, INPUT);

  pinMode(MotorPWM2, OUTPUT);
  mcp.pinMode(MotorA2, OUTPUT);
  mcp.pinMode(MotorB2, OUTPUT);
  pinMode(Lift2DownSwitch, INPUT);
  pinMode(Lift2UpSwitch, INPUT);

  pinMode(MotorPWM3, OUTPUT);
  mcp.pinMode(MotorA3, OUTPUT);
  mcp.pinMode(MotorB3, OUTPUT);
  pinMode(Lift3DownSwitch, INPUT);
  pinMode(Lift3UpSwitch, INPUT);

  pinMode(MotorPWM4, OUTPUT);
  mcp.pinMode(MotorA4, OUTPUT);
  mcp.pinMode(MotorB4, OUTPUT);

  pinMode(MotorPWM5, OUTPUT);
  mcp.pinMode(MotorA5, OUTPUT);
  mcp.pinMode(MotorB5, OUTPUT);

  pinMode(MotorPWM6, OUTPUT);
  mcp.pinMode(MotorA6, OUTPUT);
  mcp.pinMode(MotorB6, OUTPUT);

  pinMode(MotorPWM7, OUTPUT);
  mcp.pinMode(MotorA7, OUTPUT);
  mcp.pinMode(MotorB7, OUTPUT);
}

void loop() {
  //take care of the server first
  ws.cleanupClients();
  //then do our other stuff

  //the following code is essentially copied/pasted over and over again, but they all run the motors and read the limit switches

  //run lift motor 1
  if (Motor1Direction == 1 && (digitalRead(Lift1DownSwitch) != HIGH)) {
    mcp.digitalWrite(MotorA1, HIGH);
    mcp.digitalWrite(MotorB1, LOW);
    analogWrite(MotorPWM1, Motor1Speed);
  } else if (Motor1Direction == 2 && (digitalRead(Lift1UpSwitch) != HIGH)) {
    mcp.digitalWrite(MotorA1, LOW);
    mcp.digitalWrite(MotorB1, HIGH);
    analogWrite(MotorPWM1, Motor1Speed);
  } else {
    mcp.digitalWrite(MotorA1, LOW);
    mcp.digitalWrite(MotorB1, LOW);
  }
  //run lift motor 2
  if (Motor2Direction == 2 && (digitalRead(Lift2DownSwitch) != HIGH)) {
    mcp.digitalWrite(MotorA2, HIGH);
    mcp.digitalWrite(MotorB2, LOW);
    analogWrite(MotorPWM2, Motor2Speed);
  } else if (Motor2Direction == 1 && (digitalRead(Lift2UpSwitch) != HIGH)) {
    mcp.digitalWrite(MotorA2, LOW);
    mcp.digitalWrite(MotorB2, HIGH);
    analogWrite(MotorPWM2, Motor2Speed);
  } else {
    mcp.digitalWrite(MotorA2, LOW);
    mcp.digitalWrite(MotorB2, LOW);
  }
  //run lift motor 3
  if (Motor3Direction == 2 && (digitalRead(Lift3DownSwitch) != HIGH)) {
    mcp.digitalWrite(MotorA3, HIGH);
    mcp.digitalWrite(MotorB3, LOW);
    analogWrite(MotorPWM3, Motor3Speed);
  } else if (Motor3Direction == 1 && (digitalRead(Lift3UpSwitch) != HIGH)) {
    mcp.digitalWrite(MotorA3, LOW);
    mcp.digitalWrite(MotorB3, HIGH);
    analogWrite(MotorPWM3, Motor3Speed);
  } else {
    mcp.digitalWrite(MotorA3, LOW);
    mcp.digitalWrite(MotorB3, LOW);
  }
  //run motor 4
  if (Motor4Direction == 1) {
    mcp.digitalWrite(MotorA4, HIGH);
    mcp.digitalWrite(MotorB4, LOW);
    analogWrite(MotorPWM4, Motor4Speed);
  } else if (Motor4Direction == 2) {
    mcp.digitalWrite(MotorA4, LOW);
    mcp.digitalWrite(MotorB4, HIGH);
    analogWrite(MotorPWM4, Motor4Speed);
  } else {
    mcp.digitalWrite(MotorA4, LOW);
    mcp.digitalWrite(MotorB4, LOW);
  }
  //run motor 5
  if (Motor5Direction == 2) {
    mcp.digitalWrite(MotorA5, HIGH);
    mcp.digitalWrite(MotorB5, LOW);
    analogWrite(MotorPWM5, Motor5Speed);
  } else if (Motor5Direction == 1) {
    mcp.digitalWrite(MotorA5, LOW);
    mcp.digitalWrite(MotorB5, HIGH);
    analogWrite(MotorPWM5, Motor5Speed);
  } else {
    mcp.digitalWrite(MotorA5, LOW);
    mcp.digitalWrite(MotorB5, LOW);
  }
  //run motor 6
  if (Motor6Direction == 1) {
    mcp.digitalWrite(MotorA6, HIGH);
    mcp.digitalWrite(MotorB6, LOW);
    analogWrite(MotorPWM6, Motor6Speed);
  } else if (Motor6Direction == 2) {
    mcp.digitalWrite(MotorA6, LOW);
    mcp.digitalWrite(MotorB6, HIGH);
    analogWrite(MotorPWM6, Motor6Speed);
  } else {
    mcp.digitalWrite(MotorA6, LOW);
    mcp.digitalWrite(MotorB6, LOW);
  }
  //run motor 7
  if (Motor7Direction == 2) {
    mcp.digitalWrite(MotorA7, HIGH);
    mcp.digitalWrite(MotorB7, LOW);
    analogWrite(MotorPWM7, Motor7Speed);
  } else if (Motor7Direction == 1) {
    mcp.digitalWrite(MotorA7, LOW);
    mcp.digitalWrite(MotorB7, HIGH);
    analogWrite(MotorPWM7, Motor7Speed);
  } else {
    mcp.digitalWrite(MotorA7, LOW);
    mcp.digitalWrite(MotorB7, LOW);
  }
  //don't forget our servo too. We only ended up using this oen in the final design
  pickup.write(PickupAngle);
}
