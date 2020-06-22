///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////         ///////////////////////
/////////////////////////    ESP 32   /////////////////////
///////////////////////////         ///////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

///////////////////////////
//////               //////
//////   WIFI ESP32  //////
//////               //////
///////////////////////////
#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <ESP8266WebServer.h>
#include <ESPAsyncTCP.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <WebSocketsServer.h>
ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(1337);
#else

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <AsyncTCP.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>
#include <FS.h>
#include <WebSocketsServer.h>
#include <ArduinoOTA.h>
//WebServer server(80);
//WebSocketsServer webSocket = WebSocketsServer(81);

#endif

#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>

AsyncWebServer server(80);

WebSocketsServer webSocket = WebSocketsServer(81);
char msg_buf[10];
int led_state = 0;

AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  if (type == WS_EVT_CONNECT)
  {
    Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
    client->printf("Hello Client %u :)", client->id());
    client->ping();
  }
  else if (type == WS_EVT_DISCONNECT)
  {
    Serial.printf("ws[%s][%u] disconnect\n", server->url(), client->id());
  }
  else if (type == WS_EVT_ERROR)
  {
    Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t *)arg), (char *)data);
  }
  else if (type == WS_EVT_PONG)
  {
    Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char *)data : "");
  }
  else if (type == WS_EVT_DATA)
  {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    String msg = "";
    if (info->final && info->index == 0 && info->len == len)
    {
      //the whole message is in a single frame and we got all of it's data
      Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);

      if (info->opcode == WS_TEXT)
      {
        for (size_t i = 0; i < info->len; i++)
        {
          msg += (char)data[i];
        }
      }
      else
      {
        char buff[3];
        for (size_t i = 0; i < info->len; i++)
        {
          sprintf(buff, "%02x ", (uint8_t)data[i]);
          msg += buff;
        }
      }
      Serial.printf("%s\n", msg.c_str());

      if (info->opcode == WS_TEXT)
        client->text("I got your text message");
      else
        client->binary("I got your binary message");
    }
    else
    {
      //message is comprised of multiple frames or the frame is split into multiple packets
      if (info->index == 0)
      {
        if (info->num == 0)
          Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
        Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
      }

      Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT) ? "text" : "binary", info->index, info->index + len);

      if (info->opcode == WS_TEXT)
      {
        for (size_t i = 0; i < len; i++)
        {
          msg += (char)data[i];
        }
      }
      else
      {
        char buff[3];
        for (size_t i = 0; i < len; i++)
        {
          sprintf(buff, "%02x ", (uint8_t)data[i]);
          msg += buff;
        }
      }
      Serial.printf("%s\n", msg.c_str());

      if ((info->index + len) == info->len)
      {
        Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
        if (info->final)
        {
          Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
          if (info->message_opcode == WS_TEXT)
            client->text("I got your text message");
          else
            client->binary("I got your binary message");
        }
      }
    }
  }
}
// Constants
const char *msg_toggle_led = "toggleLED";
const char *msg_get_led = "getLEDState";
const int dns_port = 53;
const int http_port = 80;
const int ws_port = 1337;
const int led_pin = 15;

const char *ssid = "Matheus";
const char *password = "94777463";
const char *hostName = "esp-async";

const char *ssidAP = "Narnia";
const char *passAP = "12345678";

const char *http_username = "admin";
const char *http_password = "admin";

///////////////////////////
//////               //////
//////  BIBLIOTECAS  //////
//////               //////
///////////////////////////

// Bibliotecas Sensores
#include <OneWire.h>           // Biblioteca Varios devices em One Wire
#include <DallasTemperature.h> // Biblioteca Sensores Temperatura DS18b20
#include "DHTesp.h"            // Biblioteca Sensor temperatura e umidade DHT11

// Biblioteca Times
#include <NTPClient.h>

///////////////////////////
//                       //
// DEFINICOES DE PINOS   //
//                       //
///////////////////////////
#define DHTpin 5             // D5 of NodeMCU is GPIO14
#define sLuz 19              // Sensor de luminosidade
#define sensorCombustivel 34 // Sensor potenciometro Sensor de combustível
#define ONE_WIRE_BUS 2       // Sensores de Temperatura ONEWIRE
#define LED_RED 25           // specify the pins with an RGB LED connected
#define LED_GREEN 33
#define LED_BLUE 32

///////////////////////////
//////               //////
//////  DEFINIÇÕES   //////
//////               //////
///////////////////////////

#define DBG_OUTPUT_PORT Serial // Definindo Nome do Serial de monitoramento

/*______________________________ OneWIRE ________________________________*/
OneWire oneWire(ONE_WIRE_BUS);       // Criando Objeto OneWire
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.
int numberOfDevices;                 // Number of temperature devices found in OneWire
DeviceAddress tempDeviceAddress;     // We'll use this variable to store a found device address

DHTesp dht;

/*______________________________ Declarando Relogio _____________________*/
WiFiUDP udp;
NTPClient ntp(udp, "a.st1.ntp.br", -3 * 3600, 60000); //Cria um objeto "NTP" com as configurações.utilizada no Brasil
String hora = "";
String horaNova = "";

///////////////////////////
//////               //////
//////  VARIÁVEIS    //////
//////               //////
///////////////////////////

String Leitura;
String mensagemGlobal = "";

bool DESLIGADO = 1;
bool LIGADO = 0;

int contador = 0;

int lastTime = 0;
int lastTime1 = 0;

int umidadeLocal = 0;
int temperaturaLocal = 0;
int leituraCombustivel = 0;
int nivelCombustivel = 0;

// Temperatura tanque 01
int TempTanque01 = 0;
int TempMaxTanque01 = 0;
String TimetTempMaxTanque01 = "";
int TempMinTanque01 = 0;
String TimetTempMinTanque01 = "";

// Temperatura tanque 02
int TempTanque02 = 0;
int TempMaxTanque02 = 0;
String TimetTempMaxTanque02 = "";
int TempMinTanque02 = 0;
String TimetTempMinTanque02 = "";

// Luminosidade
String luminosidadeLocal;

/***********************************************************
   Functions
*/

// Callback: receiving any WebSocket message
void onWebSocketEvent(uint8_t client_num,
                      WStype_t type,
                      uint8_t *payload,
                      size_t length)
{

  // Figure out the type of WebSocket event
  switch (type)
  {

    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", client_num);
      break;

    // New client has connected
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(client_num);
        Serial.printf("[%u] Connection from ", client_num);
        Serial.println(ip.toString());
      }
      break;

    // Handle text messages from client
    case WStype_TEXT:

      // Print out raw message
      Serial.printf("[%u] Received text: %s\n", client_num, payload);
      Leitura = ((char *)payload);
      Serial.println(Leitura);

      if (Leitura == "myonoffswitch1ligado")
      {
        digitalWrite(LED_RED, LIGADO); // turn off the LEDs
      }
      if (Leitura == "myonoffswitch1desligado")
      {
        digitalWrite(LED_RED, DESLIGADO); // turn off the LEDs
      }
      if (Leitura == "myonoffswitch2ligado")
      {
        digitalWrite(LED_GREEN, LIGADO); // turn off the LEDs
      }
      if (Leitura == "myonoffswitch2desligado")
      {
        digitalWrite(LED_GREEN, DESLIGADO); // turn off the LEDs
      }
      if (Leitura == "myonoffswitch3ligado")
      {
        digitalWrite(LED_BLUE, LIGADO); // turn off the LEDs
      }
      if (Leitura == "myonoffswitch3desligado")
      {
        digitalWrite(LED_BLUE, DESLIGADO); // turn off the LEDs
      }
      if (Leitura == "myonoffswitch4ligado")
      {
        digitalWrite(LED_RED, LIGADO); // turn off the LEDs
      }
      if (Leitura == "myonoffswitch4desligado")
      {
        digitalWrite(LED_RED, DESLIGADO); // turn off the LEDs
      }

      // Toggle LED
      if (strcmp((char *)payload, "toggleLED") == 0)
      {
        led_state = led_state ? 0 : 1;
        Serial.printf("Toggling LED to %u\n", led_state);
        digitalWrite(led_pin, led_state);

        // Report the state of the LED
      }
      else if (strcmp((char *)payload, "getLEDState") == 0)
      {
        sprintf(msg_buf, "%d", led_state);
        Serial.printf("Sending to [%u]: %s\n", client_num, msg_buf);
        webSocket.sendTXT(client_num, msg_buf);

        // Message not recognized
      }
      else
      {
        Serial.println("[%u] Message not recognized");
        Serial.println(Leitura);
      }
      break;

    // For everything else: do nothing
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }
}

// Callback: send homepage
void onIndexRequest(AsyncWebServerRequest *request)
{
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/index.html", "text/html");
}

// Callback: send style sheet
void onCSSRequest(AsyncWebServerRequest *request)
{
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/style.css", "text/css");
}

// Callback: send 404 if requested file does not exist
void onPageNotFound(AsyncWebServerRequest *request)
{
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(404, "text/plain", "Not found");
}

/***********************************************************
 ** Main
*/

///////////////////////////
//////               //////
////// SYS ARQUIVOS  //////
//////               //////
///////////////////////////
/*
  File fsUploadFile;             // Definindo nome do sistema de arquivos
  //format bytes
  String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  } else {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
  }

  String getContentType(String filename) {
  if (server.hasArg("download")) return "application/octet-stream";
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
  }

  bool handleFileRead(String path) {
  DBG_OUTPUT_PORT.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.htm";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    if (SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
  }

  void handleFileUpload() {
  if (server.uri() != "/edit") return;
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) filename = "/" + filename;
    DBG_OUTPUT_PORT.print("handleFileUpload Name: "); DBG_OUTPUT_PORT.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    //DBG_OUTPUT_PORT.print("handleFileUpload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile)
      fsUploadFile.close();
    DBG_OUTPUT_PORT.print("handleFileUpload Size: "); DBG_OUTPUT_PORT.println(upload.totalSize);
  }
  }

  void handleFileDelete() {
  if (server.args() == 0) return server.send(500, "text/plain", "BAD ARGS");
  String path = server.arg(0);
  DBG_OUTPUT_PORT.println("handleFileDelete: " + path);
  if (path == "/")
    return server.send(500, "text/plain", "BAD PATH");
  if (!SPIFFS.exists(path))
    return server.send(404, "text/plain", "FileNotFound");
  SPIFFS.remove(path);
  server.send(200, "text/plain", "");
  path = String();
  }

  void handleFileCreate() {
  if (server.args() == 0)
    return server.send(500, "text/plain", "BAD ARGS");
  String path = server.arg(0);
  DBG_OUTPUT_PORT.println("handleFileCreate: " + path);
  if (path == "/")
    return server.send(500, "text/plain", "BAD PATH");
  if (SPIFFS.exists(path))
    return server.send(500, "text/plain", "FILE EXISTS");
  File file = SPIFFS.open(path, "w");
  if (file)
    file.close();
  else
    return server.send(500, "text/plain", "CREATE FAILED");
  server.send(200, "text/plain", "");
  path = String();
  }

  void returnFail(String msg) {
  server.send(500, "text/plain", msg + "\r\n");
  }

  #ifdef ESP8266
  void handleFileList() {
  if (!server.hasArg("dir")) {
    returnFail("BAD ARGS");
    return;
  }

  String path = server.arg("dir");
  DBG_OUTPUT_PORT.println("handleFileList: " + path);
  Dir dir = SPIFFS.openDir(path);
  path = String();

  String output = "[";
  while (dir.next()) {
    File entry = dir.openFile("r");
    if (output != "[") output += ',';
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += String(entry.name()).substring(1);
    output += "\"}";
    entry.close();
  }

  output += "]";
  server.send(200, "text/json", output);
  }
  #else
  void handleFileList() {
  if (!server.hasArg("dir")) {
    returnFail("BAD ARGS");
    return;
  }
  String path = server.arg("dir");
  if (path != "/" && !SPIFFS.exists((char *)path.c_str())) {
    returnFail("BAD PATH");
    return;
  }
  File dir = SPIFFS.open((char *)path.c_str());
  path = String();
  if (!dir.isDirectory()) {
    dir.close();
    returnFail("NOT DIR");
    return;
  }
  dir.rewindDirectory();

  String output = "[";
  for (int cnt = 0; true; ++cnt) {
    File entry = dir.openNextFile();
    if (!entry)
      break;

    if (cnt > 0)
      output += ',';

    output += "{\"type\":\"";
    output += (entry.isDirectory()) ? "dir" : "file";
    output += "\",\"name\":\"";
    // Ignore '/' prefix
    output += entry.name() + 1;
    output += "\"";
    output += "}";
    entry.close();
  }
  output += "]";
  server.send(200, "text/json", output);
  dir.close();
  }

  void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  DBG_OUTPUT_PORT.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    DBG_OUTPUT_PORT.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    DBG_OUTPUT_PORT.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      DBG_OUTPUT_PORT.print("  DIR : ");
      DBG_OUTPUT_PORT.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      DBG_OUTPUT_PORT.print("  FILE: ");
      DBG_OUTPUT_PORT.print(file.name());
      DBG_OUTPUT_PORT.print("  SIZE: ");
      DBG_OUTPUT_PORT.println(file.size());
    }
    file = root.openNextFile();
  }
  }
*/

////////////////////////////
//////                //////
////// FUNCÃO WSOCKET //////
//////                //////
////////////////////////////
/*

  void startWebSocket()
  { // Start a WebSocket server
  webSocket.begin();                 // start the websocket server
  webSocket.onEvent(webSocketEvent); // if there's an incomming websocket message, go to function 'webSocketEvent'
  Serial.println("WebSocket server started.");
  }


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////// When a WebSocket message is received  /////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t lenght)
  { // When a WebSocket message is received
  switch (type)
  {

    case WStype_DISCONNECTED: // if the websocket is disconnected
      Serial.printf("[%u] Disconnected!\n", num);
      break;

    case WStype_CONNECTED:
      { // if a new websocket connection is established
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        //rainbow = false; // Turn rainbow off when a new connection is established
      }
      break;

    case WStype_TEXT: // if new text data is received
      Leitura = ((char *)payload);
      Serial.println(Leitura);

      if (Leitura == "myonoffswitch1ligado")
      {
        digitalWrite(LED_RED, LIGADO); // turn off the LEDs
      }
      if (Leitura == "myonoffswitch1desligado")
      {
        digitalWrite(LED_RED, DESLIGADO); // turn off the LEDs
      }
      if (Leitura == "myonoffswitch2ligado")
      {
        digitalWrite(LED_GREEN, LIGADO); // turn off the LEDs
      }
      if (Leitura == "myonoffswitch2desligado")
      {
        digitalWrite(LED_GREEN, DESLIGADO); // turn off the LEDs
      }
      if (Leitura == "myonoffswitch3ligado")
      {
        digitalWrite(LED_BLUE, LIGADO); // turn off the LEDs
      }
      if (Leitura == "myonoffswitch3desligado")
      {
        digitalWrite(LED_BLUE, DESLIGADO); // turn off the LEDs
      }
      if (Leitura == "myonoffswitch4ligado")
      {
        digitalWrite(LED_RED, LIGADO); // turn off the LEDs
      }
      if (Leitura == "myonoffswitch4desligado")
      {
        digitalWrite(LED_RED, DESLIGADO); // turn off the LEDs
      }

      break;
  }
  }
  #endif


*/
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////               /////////////////////////////////
/////////////////////////////////     SETUP     /////////////////////////////////
/////////////////////////////////               /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
void setup(void)
{

  // Init LED and turn off
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);

  ///////////////////////////
  //////               //////
  ////// BEGIN SERIAL  //////
  //////               //////
  ///////////////////////////
  DBG_OUTPUT_PORT.begin(115200);
  DBG_OUTPUT_PORT.print("\n");
  DBG_OUTPUT_PORT.setDebugOutput(true);

  ///////////////////////////
  //////               //////
  ////// BEGIN PINOS   //////
  //////               //////
  ///////////////////////////
  pinMode(sLuz, INPUT);
  pinMode(LED_RED, OUTPUT); // the pins with LEDs connected are outputs
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_RED, DESLIGADO); // turn off the LEDs
  digitalWrite(LED_GREEN, DESLIGADO);
  digitalWrite(LED_BLUE, DESLIGADO);

  ///////////////////////////
  //////               //////
  ////// BEGIN ONEWIRE //////
  //////               //////
  ///////////////////////////
  sensors.begin();                            // Start up the library - Sensores de temeratura oneWire
  numberOfDevices = sensors.getDeviceCount(); // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");
  dht.setup(DHTpin, DHTesp::DHT11); // Iniciando sensor DHT temp. Umid.

  ////////////////////////////
  //////                //////
  ////// BEGIN SYS FILE //////
  //////                //////
  ////////////////////////////
  /*
    SPIFFS.begin();
    {
    #ifdef ESP8266
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      DBG_OUTPUT_PORT.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    #else
    listDir(SPIFFS, "/", 0);
    #endif
    DBG_OUTPUT_PORT.printf("\n");
    }
  */

  ///////////////////////////
  //////               //////
  //////  BEGIN WIFI   //////
  //////               //////
  ///////////////////////////
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(hostName);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.printf("STA: Failed!\n");
    WiFi.disconnect(false);
    delay(1000);
    WiFi.begin(ssid, password);
  }

  // On HTTP request for root, provide index.html file
  server.on("/", HTTP_GET, onIndexRequest);

  // On HTTP request for style sheet, provide style.css
  server.on("/style.css", HTTP_GET, onCSSRequest);

  // Handle requests for pages that do not exist
  server.onNotFound(onPageNotFound);

  // Start web server
  server.begin();

  // Start WebSocket server and assign callback
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);

  /*
    WiFi.softAP(ssidAP, passAP);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);

    ///
    DBG_OUTPUT_PORT.printf("Connecting to %s\n", ssid);
    if (String(WiFi.SSID()) != String(ssid)) {
    WiFi.begin(ssid, password);
    }

    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DBG_OUTPUT_PORT.print(".");
    }
    DBG_OUTPUT_PORT.println("");
    DBG_OUTPUT_PORT.print("Connected! IP address: ");
    DBG_OUTPUT_PORT.println(WiFi.localIP());

    MDNS.begin(host);
    DBG_OUTPUT_PORT.print("Open http://");
    DBG_OUTPUT_PORT.print(host);
    DBG_OUTPUT_PORT.println(".local/edit to see the file browser");
  */

  ///////////////////////////
  //////               //////
  ////// BEGIN OTA     //////
  //////               //////
  ///////////////////////////

  //Send OTA events to the browser
  ArduinoOTA.onStart([]() {
    events.send("Update Start", "ota");
  });
  ArduinoOTA.onEnd([]() {
    events.send("Update End", "ota");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    char p[32];
    sprintf(p, "Progress: %u%%\n", (progress / (total / 100)));
    events.send(p, "ota");
  });
  ArduinoOTA.onError([](ota_error_t error) {
    if (error == OTA_AUTH_ERROR)
      events.send("Auth Failed", "ota");
    else if (error == OTA_BEGIN_ERROR)
      events.send("Begin Failed", "ota");
    else if (error == OTA_CONNECT_ERROR)
      events.send("Connect Failed", "ota");
    else if (error == OTA_RECEIVE_ERROR)
      events.send("Recieve Failed", "ota");
    else if (error == OTA_END_ERROR)
      events.send("End Failed", "ota");
  });
  ArduinoOTA.setHostname(hostName);
  ArduinoOTA.begin();

  ///////////////////////////
  //////               //////
  ////// BEGIN ssifp novo sabado  //////
  //////               //////
  ///////////////////////////

  MDNS.addService("http", "tcp", 80);

  SPIFFS.begin();

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  events.onConnect([](AsyncEventSourceClient * client) {
    client->send("hello!", NULL, millis(), 1000);
  });
  server.addHandler(&events);

#ifdef ESP32
  server.addHandler(new SPIFFSEditor(SPIFFS, http_username, http_password));
#elif defined(ESP8266)
  server.addHandler(new SPIFFSEditor(http_username, http_password));
#endif

  server.on("/heap", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", String(ESP.getFreeHeap()));
  });

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("indexRoca.html");

  server.onNotFound([](AsyncWebServerRequest * request) {
    Serial.printf("NOT_FOUND: ");
    if (request->method() == HTTP_GET)
      Serial.printf("GET");
    else if (request->method() == HTTP_POST)
      Serial.printf("POST");
    else if (request->method() == HTTP_DELETE)
      Serial.printf("DELETE");
    else if (request->method() == HTTP_PUT)
      Serial.printf("PUT");
    else if (request->method() == HTTP_PATCH)
      Serial.printf("PATCH");
    else if (request->method() == HTTP_HEAD)
      Serial.printf("HEAD");
    else if (request->method() == HTTP_OPTIONS)
      Serial.printf("OPTIONS");
    else
      Serial.printf("UNKNOWN");
    Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

    if (request->contentLength())
    {
      Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
      Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
    }

    int headers = request->headers();
    int i;
    for (i = 0; i < headers; i++)
    {
      AsyncWebHeader *h = request->getHeader(i);
      Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    }

    int params = request->params();
    for (i = 0; i < params; i++)
    {
      AsyncWebParameter *p = request->getParam(i);
      if (p->isFile())
      {
        Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      }
      else if (p->isPost())
      {
        Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
      else
      {
        Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }

    request->send(404);
  });
  server.onFileUpload([](AsyncWebServerRequest * request, const String & filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index)
      Serial.printf("UploadStart: %s\n", filename.c_str());
    Serial.printf("%s", (const char *)data);
    if (final)
      Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index + len);
  });
  server.onRequestBody([](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
    if (!index)
      Serial.printf("BodyStart: %u\n", total);
    Serial.printf("%s", (const char *)data);
    if (index + len == total)
      Serial.printf("BodyEnd: %u\n", total);
  });
  server.begin();

  ///////////////////////////
  //////               //////
  ////// BEGIN SERVER  //////
  //////               //////
  ///////////////////////////

  /*
    //list directory
    server.on("/list", HTTP_GET, handleFileList);
    //load editor
    server.on("/edit", HTTP_GET, []() {
    if (!handleFileRead("/edit.htm")) server.send(404, "text/plain", "FileNotFound");
    });
    //create file
    server.on("/edit", HTTP_PUT, handleFileCreate);
    //delete file
    server.on("/edit", HTTP_DELETE, handleFileDelete);
    //first callback is called after the request has ended with all parsed arguments
    //second callback handles file uploads at that location
    server.on("/edit", HTTP_POST, []() {
    server.send(200, "text/plain", "");
    }, handleFileUpload);

    //called when the url is not defined here
    //use it to load content from SPIFFS
    server.onNotFound([]() {
    if (!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
    });

    //get heap status, analog input value and all GPIO statuses in one json call
    server.on("/all", HTTP_GET, []() {
    String json = "{";
    json += "\"heap\":" + String(ESP.getFreeHeap());
    json += ", \"analog\":" + String(analogRead(A0));
    #ifdef ESP8266
    json += ", \"gpio\":" + String((uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16)));
    #endif
    json += "}";
    server.send(200, "text/json", json);
    json = String();
    });
    server.begin();
    DBG_OUTPUT_PORT.println("HTTP server started");
  */
  ///////////////////////////
  //////               //////
  //////  BEGIN CLOCK  //////
  //////               //////
  ///////////////////////////

  ntp.begin();          // Inicia o protocolo
  ntp.forceUpdate();    // Atualização .
  hora = ntp.getFormattedTime();
  Serial.println(hora);
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////               /////////////////////////////////
/////////////////////////////////      LOOP     /////////////////////////////////
/////////////////////////////////               /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
void loop(void)
{

  ///////////////////////////
  //////               //////
  ////// UPDATE CLOCK  //////
  //////               //////
  ///////////////////////////
  if ((millis() - lastTime1) > 10000)
  {
    lastTime1 = millis();
    hora = ntp.getFormattedTime();
  }
  /*
     ///////////////////////////
     //////               //////
     ////// LOOP SERVER   //////
     //////               //////
     ///////////////////////////
     //------------------------------------- Loop Servidor ---------------------------------
     if ((millis() - lastTime) > 1000)
     {
       // Lendo nível combustível
       leituraCombustivel = analogRead(sensorCombustivel);
       nivelCombustivel = map(leituraCombustivel, 0, 1023, 0, 100);

       // Lendo luminosidade
       if (digitalRead(sLuz) == HIGH)
       {
         luminosidadeLocal = "Baixa";
       }
       else
       {
         luminosidadeLocal = "Alta";
       }

       delay(dht.getMinimumSamplingPeriod());

       float humidity = dht.getHumidity();
       float temperature = dht.getTemperature();

       temperaturaLocal = temperature;
       umidadeLocal = humidity;

       //Lendo Temperaturas Tanques
       sensors.requestTemperatures(); // Send the command to get temperatures
       // Loop through each device, print out temperature data
       for (int i = 0; i < numberOfDevices; i++)
       {
         // Search the wire for address
         if (sensors.getAddress(tempDeviceAddress, i))
         {

           float tempC = sensors.getTempC(tempDeviceAddress);
           switch (i)
           {
             case 0:
               TempTanque02 = tempC;
               break;

             case 1:
               TempTanque01 = tempC;
               break;

             default:
               break;
           }
         }
       }

       // Atualização temperaturaS TANQUE 01 ////////////////////////////////////////////////
       horaNova = hora;
       if ((TempMaxTanque01 == 0) && (TempMinTanque01 == 0))
       {
         TempMinTanque01 = TempTanque01;
         TempMaxTanque01 = TempTanque01;
         horaNova.remove(5);
         TimetTempMaxTanque01 = horaNova;
         TimetTempMinTanque01 = horaNova;
       }
       if (TempTanque01 > TempMaxTanque01)
       {
         TempMaxTanque01 = TempTanque01;
         horaNova.remove(5);
         TimetTempMaxTanque01 = horaNova;
       }
       else if (TempTanque01 < TempMinTanque01)
       {
         TempMinTanque01 = TempTanque01;
         horaNova.remove(5);
         TimetTempMinTanque01 = horaNova;
       }

       if ((TimetTempMinTanque01 == "") && (TimetTempMaxTanque01 = ""))
       {
         horaNova.remove(5);
         TimetTempMaxTanque01 = horaNova;
         TimetTempMinTanque01 = horaNova;
       }

       // Atualização temperaturaS TANQUE 02 ////////////////////////////////////////////////
       horaNova = String(hora);

       if ((TempMaxTanque02 == 0) && (TempMinTanque02 == 0))
       {
         TempMinTanque02 = TempTanque02;
         TempMaxTanque02 = TempTanque02;
         horaNova.remove(5);
         TimetTempMaxTanque02 = horaNova;
         TimetTempMinTanque02 = horaNova;
       }
       if (TempTanque02 > TempMaxTanque02)
       {
         TempMaxTanque02 = TempTanque02;
         horaNova.remove(5);
         TimetTempMaxTanque02 = horaNova;
       }
       else if (TempTanque02 < TempMinTanque02)
       {
         TempMinTanque02 = TempTanque02;
         horaNova.remove(5);
         TimetTempMinTanque02 = horaNova;
       }

       if ((TimetTempMinTanque02 == "") && (TimetTempMaxTanque02 = ""))
       {
         horaNova.remove(5);
         TimetTempMaxTanque02 = horaNova;
         TimetTempMinTanque02 = horaNova;
       }

       // Envio da mensagem com dados para o servidor
       contador++;
       String message = (String("T") +                              // 00
                         String("KK") + String(temperaturaLocal) +  // 01
                         String("KK") + String("°C") +              // 02
                         String("KK") + String("U") +               // 03
                         String("KK") + String(umidadeLocal) +      // 04
                         String("KK") + String("%") +               // 05
                         String("KK") + String("L") +               // 06
                         String("KK") + String(luminosidadeLocal) + // 07
                         String("KK") + String(" lm") +             // 08
                         String("KK") + String(nivelCombustivel) +  // 09
                         String("KK") + String(hora) +              // 10

                         String("KK") + String(TempTanque01) +         // 11
                         String("KK") + String(TempMaxTanque01) +      // 12
                         String("KK") + String(TimetTempMaxTanque01) + // 13
                         String("KK") + String(TempMinTanque01) +      // 14
                         String("KK") + String(TimetTempMinTanque01) + // 15

                         String("KK") + String(TempTanque02) +         // 16
                         String("KK") + String(TempMaxTanque02) +      // 17
                         String("KK") + String(TimetTempMaxTanque02) + // 18
                         String("KK") + String(TempMinTanque02) +      // 19
                         String("KK") + String(TimetTempMinTanque02)); // 20

       webSocket.broadcastTXT(message);
       lastTime = millis();
     }

     webSocket.loop();      // constantly check for websocket events
     server.handleClient();
  */
  // Look for and handle WebSocket data

  ///////////////////////////
  //////               //////
  ////// LOOP SERVER   //////
  //////               //////
  ///////////////////////////
  //------------------------------------- Loop Servidor ---------------------------------
  if ((millis() - lastTime) > 3000)
  {
    // Lendo nível combustível
    leituraCombustivel = analogRead(sensorCombustivel);
    nivelCombustivel = map(leituraCombustivel, 0, 1023, 0, 100);

    // Lendo luminosidade
    if (digitalRead(sLuz) == HIGH)
    {
      luminosidadeLocal = "Baixa";
    }
    else
    {
      luminosidadeLocal = "Alta";
    }

    delay(dht.getMinimumSamplingPeriod());

    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();

    temperaturaLocal = temperature;
    umidadeLocal = humidity;

    //Lendo Temperaturas Tanques
    sensors.requestTemperatures(); // Send the command to get temperatures
    // Loop through each device, print out temperature data
    for (int i = 0; i < numberOfDevices; i++)
    {
      // Search the wire for address
      if (sensors.getAddress(tempDeviceAddress, i))
      {

        float tempC = sensors.getTempC(tempDeviceAddress);
        switch (i)
        {
          case 0:
            TempTanque02 = tempC;
            break;

          case 1:
            TempTanque01 = tempC;
            break;

          default:
            break;
        }
      }
    }

    // Atualização temperaturaS TANQUE 01 ////////////////////////////////////////////////
    horaNova = hora;
    if ((TempMaxTanque01 == 0) && (TempMinTanque01 == 0))
    {
      TempMinTanque01 = TempTanque01;
      TempMaxTanque01 = TempTanque01;
      horaNova.remove(5);
      TimetTempMaxTanque01 = horaNova;
      TimetTempMinTanque01 = horaNova;
    }
    if (TempTanque01 > TempMaxTanque01)
    {
      TempMaxTanque01 = TempTanque01;
      horaNova.remove(5);
      TimetTempMaxTanque01 = horaNova;
    }
    else if (TempTanque01 < TempMinTanque01)
    {
      TempMinTanque01 = TempTanque01;
      horaNova.remove(5);
      TimetTempMinTanque01 = horaNova;
    }

    if ((TimetTempMinTanque01 == "") && (TimetTempMaxTanque01 = ""))
    {
      horaNova.remove(5);
      TimetTempMaxTanque01 = horaNova;
      TimetTempMinTanque01 = horaNova;
    }

    // Atualização temperaturaS TANQUE 02 ////////////////////////////////////////////////
    horaNova = String(hora);

    if ((TempMaxTanque02 == 0) && (TempMinTanque02 == 0))
    {
      TempMinTanque02 = TempTanque02;
      TempMaxTanque02 = TempTanque02;
      horaNova.remove(5);
      TimetTempMaxTanque02 = horaNova;
      TimetTempMinTanque02 = horaNova;
    }
    if (TempTanque02 > TempMaxTanque02)
    {
      TempMaxTanque02 = TempTanque02;
      horaNova.remove(5);
      TimetTempMaxTanque02 = horaNova;
    }
    else if (TempTanque02 < TempMinTanque02)
    {
      TempMinTanque02 = TempTanque02;
      horaNova.remove(5);
      TimetTempMinTanque02 = horaNova;
    }

    if ((TimetTempMinTanque02 == "") && (TimetTempMaxTanque02 = ""))
    {
      horaNova.remove(5);
      TimetTempMaxTanque02 = horaNova;
      TimetTempMinTanque02 = horaNova;
    }

    // Envio da mensagem com dados para o servidor
    contador++;
    String message = (String("T") +                              // 00
                      String("KK") + String(temperaturaLocal) +  // 01
                      String("KK") + String("°C") +              // 02
                      String("KK") + String("U") +               // 03
                      String("KK") + String(umidadeLocal) +      // 04
                      String("KK") + String("%") +               // 05
                      String("KK") + String("L") +               // 06
                      String("KK") + String(luminosidadeLocal) + // 07
                      String("KK") + String(" lm") +             // 08
                      String("KK") + String(nivelCombustivel) +  // 09
                      String("KK") + String(hora) +              // 10

                      String("KK") + String(TempTanque01) +         // 11
                      String("KK") + String(TempMaxTanque01) +      // 12
                      String("KK") + String(TimetTempMaxTanque01) + // 13
                      String("KK") + String(TempMinTanque01) +      // 14
                      String("KK") + String(TimetTempMinTanque01) + // 15

                      String("KK") + String(TempTanque02) +         // 16
                      String("KK") + String(TempMaxTanque02) +      // 17
                      String("KK") + String(TimetTempMaxTanque02) + // 18
                      String("KK") + String(TempMinTanque02) +      // 19
                      String("KK") + String(TimetTempMinTanque02)); // 20
    mensagemGlobal = String(message);
    webSocket.broadcastTXT(mensagemGlobal);
    lastTime = millis();
  }
  webSocket.loop();
  ArduinoOTA.handle();
  ws.cleanupClients();
}
