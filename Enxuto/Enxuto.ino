#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoOTA.h> //Biblioteca Upload codigo por wifi
#include <ESP8266WebServer.h>
#include <FS.h>
#include <WebSocketsServer.h>
#include "DHTesp.h" //Biblioteca Sensor temperatura e umidade DHT11
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#define DHTpin 14 //D5 of NodeMCU is GPIO14
#define sLuz 16   // Sensor de luminosidade
#define sensorCombustivel A0

#define ONE_WIRE_BUS 2 // Sensores de Temperatura ONEWIRE

/*______________________________ OneWIRE ________________________________*/
OneWire oneWire(ONE_WIRE_BUS);       // Criando Objeto OneWire
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.
int numberOfDevices;                 // Number of temperature devices found in OneWire
DeviceAddress tempDeviceAddress;     // We'll use this variable to store a found device address

DHTesp dht;

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
ESP8266WebServer server(80);    // create a web server on port 80
WebSocketsServer webSocket(81); // create a websocket server on port 81
File fsUploadFile;              // a File variable to temporarily store the received file

/*______________________________________ WIFI Setup ______________________________________*/
const char *OTAName = "94777463"; // A name and a password for the OTA service
const char *OTAPassword = "";
const char *REDE = "Matheus";
const char *SENHA = "94777463";

#ifndef APSSID
#define APSSID "Narnia";
#define APPSK  "12345678";
#endif

/* Set these to your desired credentials. */
const char *ssidAP = APSSID;
const char *passwordAP = APPSK;

//DEFINIÇÃO DE IP FIXO PARA O NODEMCU
IPAddress ip(192, 168, 0, 11);      //COLOQUE UMA FAIXA DE IP DISPONÍVEL DO SEU ROTEADOR. EX: 192.168.1.110 **** ISSO VARIA, NO MEU CASO É: 192.168.0.175
IPAddress gateway(192, 168, 0, 1);  //GATEWAY DE CONEXÃO (ALTERE PARA O GATEWAY DO SEU ROTEADOR)
IPAddress subnet(255, 255, 255, 0); //MASCARA DE REDE

/*______________________________________ Declarações _____________________________________*/
String Leitura;

#define LED_RED 15 // specify the pins with an RGB LED connected
#define LED_GREEN 12
#define LED_BLUE 13

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

/*______________________________ Declarando Relogio _____________________*/
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "b.st1.ntp.br", -3 * 3600, 60000);
String hora = "";
String horaNova = "";

/*__________________________________________________________SETUP__________________________________________________________*/
void setup()
{

  pinMode(sLuz, INPUT);

  pinMode(LED_RED, OUTPUT); // the pins with LEDs connected are outputs
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_RED, DESLIGADO); // turn off the LEDs
  digitalWrite(LED_GREEN, DESLIGADO);
  digitalWrite(LED_BLUE, DESLIGADO);

  Serial.begin(115200); // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println("\r\n");
  Serial.println("Serial Iniciado");

  //ONE WIRE - Sensore de temperatura
  sensors.begin();                            // Start up the library - Sensores de temeratura oneWire
  numberOfDevices = sensors.getDeviceCount(); // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");
  // locate devices on the bus - ONEWIRE
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  dht.setup(DHTpin, DHTesp::DHT11);

  // Relogio
  timeClient.begin();

  WiFi.softAP(ssidAP, passwordAP);
  IPAddress myIP = WiFi.softAPIP();
  server.begin();

  delay(2000);
  startWiFi();      // Start a Wi-Fi access point, and try to connect to some given access points. Then wait for either an AP or STA connection
  startOTA();       // Start the OTA service
  startSPIFFS();    // Start the SPIFFS and list all contents
  startWebSocket(); // Start a WebSocket server
  startServer();    // Start a HTTP server with a file read handler and an upload handler
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*__________________________________________________________LOOP__________________________________________________________*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool rainbow = false; // The rainbow effect is turned off on startup

unsigned long prevMillis = millis();
int hue = 0;

time_t prevDisplay = 0; // when the digital clock was displayed

void loop()
{
  // ------------------------------------ Atualizando Relógio -------------------------------
  if ((millis() - lastTime1) > 10000)
  {
    lastTime1 = millis();
    //Relogio
    timeClient.update();
    hora = timeClient.getFormattedTime();
    Serial.println(hora);
  }

  //------------------------------------- Loop Servidor ---------------------------------
  if ((millis() - lastTime) > 1000)
  {
    // Lendo nível combustível
    leituraCombustivel = analogRead(sensorCombustivel);
    nivelCombustivel = map(leituraCombustivel, 0, 1023, 0, 100);
    Serial.println(nivelCombustivel);

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
  server.handleClient(); // run the server
  ArduinoOTA.handle();   // listen for OTA events
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*_________________________________________________ SETUP FUNCTIONS ESSENTIALS __________________________________________________________*/

void startWiFi()
{                               // Start a Wi-Fi access point, and try to connect to some given access points. Then wait for either an AP or STA connection
  wifiMulti.addAP(REDE, SENHA); // add Wi-Fi networks you want to connect to
  //WiFi.begin("Matheus", "94777463"); //PASSA OS PARÂMETROS PARA A FUNÇÃO QUE VAI FAZER A CONEXÃO COM A REDE SEM FIO
  //WiFi.config(ip, gateway, subnet);  //PASSA OS PARÂMETROS PARA A FUNÇÃO QUE VAI SETAR O IP FIXO NO NODEMCU

  Serial.println("Connecting");

  while (wifiMulti.run() != WL_CONNECTED && WiFi.softAPgetStationNum() < 1)
  { // Wait for the Wi-Fi to connect
    delay(250);
    Serial.print('.');
  }
  Serial.println("Conectado ao Wifi");
}

void startOTA()
{ // Start the OTA service
  ArduinoOTA.setHostname(OTAName);
  ArduinoOTA.setPassword(OTAPassword);

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\r\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA ready\r\n");
}
void startSPIFFS()
{                 // Start the SPIFFS and list all contents
  SPIFFS.begin(); // Start the SPI Flash File System (SPIFFS)
  Serial.println("SPIFFS started. Contents:");
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next())
    { // List the file system contents
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("\tFS File: %s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    Serial.printf("\n");
  }
}
void startWebSocket()
{                                    // Start a WebSocket server
  webSocket.begin();                 // start the websocket server
  webSocket.onEvent(webSocketEvent); // if there's an incomming websocket message, go to function 'webSocketEvent'
  Serial.println("WebSocket server started.");
}
/*________________________________________________ INICIANDO SERVIDOR _______________________________________________________________*/
void startServer()
{ // Start a HTTP server with a file read handler and an upload handler
  server.on(
      "/edit.html", HTTP_POST, []() { // If a POST request is sent to the /edit.html address,
        server.send(200, "text/plain", "");
      },
      handleFileUpload); // go to 'handleFileUpload'

  server.onNotFound(handleNotFound); // if someone requests any other file or page, go to function 'handleNotFound'
  // and check if the file exists

  server.begin(); // start the HTTP server
  Serial.println("HTTP server started.");
}
/*__________________________________________________________SERVER_NOT FOUND__________________________________________________________*/

void handleNotFound()
{ // if the requested file or page doesn't exist, return a 404 not found error
  if (!handleFileRead(server.uri()))
  { // check if the file exists in the flash memory (SPIFFS), if so, send it
    server.send(404, "text/plain", "404: File Not Found");
  }
}

/*___________________________Send the right file to the client (if it exists)________________________________________________________*/

bool handleFileRead(String path)
{ // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/"))
    path += "index.html";                    // If a folder is requested, send the index file
  String contentType = getContentType(path); // Get the MIME type
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path))
  {                                                     // If the file exists, either as a compressed archive, or normal
    if (SPIFFS.exists(pathWithGz))                      // If there's a compressed version available
      path += ".gz";                                    // Use the compressed verion
    File file = SPIFFS.open(path, "r");                 // Open the file
    size_t sent = server.streamFile(file, contentType); // Send it to the client
    file.close();                                       // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path); // If the file doesn't exist, return false
  return false;
}

/*_______________________________ Upload a new file to the SPIFFS______________________________________________________*/

void handleFileUpload()
{ // upload a new file to the SPIFFS
  HTTPUpload &upload = server.upload();
  String path;
  if (upload.status == UPLOAD_FILE_START)
  {
    path = upload.filename;
    if (!path.startsWith("/"))
      path = "/" + path;
    if (!path.endsWith(".gz"))
    {                                   // The file server always prefers a compressed version of a file
      String pathWithGz = path + ".gz"; // So if an uploaded file is not compressed, the existing compressed
      if (SPIFFS.exists(pathWithGz))    // version of that file must be deleted (if it exists)
        SPIFFS.remove(pathWithGz);
    }
    Serial.print("handleFileUpload Name: ");
    Serial.println(path);
    fsUploadFile = SPIFFS.open(path, "w"); // Open the file for writing in SPIFFS (create if it doesn't exist)
    path = String();
  }
  else if (upload.status == UPLOAD_FILE_WRITE)
  {
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
  }
  else if (upload.status == UPLOAD_FILE_END)
  {
    if (fsUploadFile)
    {                       // If the file was successfully created
      fsUploadFile.close(); // Close the file again
      Serial.print("handleFileUpload Size: ");
      Serial.println(upload.totalSize);
      server.sendHeader("Location", "/success.html"); // Redirect the client to the success page
      server.send(303);
    }
    else
    {
      server.send(500, "text/plain", "500: couldn't create file");
    }
  }
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
    rainbow = false; // Turn rainbow off when a new connection is established
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

/*__________________________________________________________HELPER_FUNCTIONS__________________________________________________________*/

String formatBytes(size_t bytes)
{ // convert sizes in bytes to KB and MB
  if (bytes < 1024)
  {
    return String(bytes) + "B";
  }
  else if (bytes < (1024 * 1024))
  {
    return String(bytes / 1024.0) + "KB";
  }
  else if (bytes < (1024 * 1024 * 1024))
  {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  }
}

String getContentType(String filename)
{ // determine the filetype of a given filename, based on the extension
  if (filename.endsWith(".html"))
    return "text/html";
  else if (filename.endsWith(".css"))
    return "text/css";
  else if (filename.endsWith(".js"))
    return "application/javascript";
  else if (filename.endsWith(".ico"))
    return "image/x-icon";
  else if (filename.endsWith(".gz"))
    return "application/x-gzip";
  return "text/plain";
}

void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16)
      Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
