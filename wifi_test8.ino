/* We can do it*/

#include <ArduinoJson.h>
#include "WiFiEsp.h"
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(2, 3); // RX, TX
#endif

char ssid[] = "Energy Lab";     // your network SSID (name)
char pwd[] = "forcfd4410";  // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

char server[] = "joljak-back.hsoo.co.kr";

// Initialize the Ethernet client object
WiFiEspClient client;

void setup()
{
  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  Serial1.begin(9600);
  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println(F("WiFi shield not present"));
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print(F("Attempting to connect to WPA SSID: "));
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pwd);
  }

  // you're connected now, so print out the data
  Serial.println(F("You're connected to the network"));
  
  printWifiStatus();

  Serial.println();
  Serial.println(F("Starting connection to server..."));
  // if you get a connection, report back via serial

  
  //Serial.println(client.connect(server, 80));
  if (client.connect(server, 443)) {
    Serial.println(F("Connected to server"));
    
    String jsondata = "";
    StaticJsonBuffer<250> jsonBuffer;  //json변환에 필요한 버퍼 생성
    JsonObject& root = jsonBuffer.createObject(); //json형식을 만들어 낼수있는 객체 생성
    root["key"] = "d0f35c1a-21f5-4300-baf5-f8573b6da12d";
    root["batteryText"] = "2h 22min";
    root["batteryPercent"] = 22;
    root["fuelCellText"] = "2h 22min";
    root["fuelCellPercent"] = 22;
    root["efficientText"] = "20%";
    root["efficientPercent"] = 22;
    root["statusText"] = "s1";

    root.printTo(jsondata); // printTo => String변수로 변환
    Serial.print("jsondata: ");
    Serial.println(jsondata);
    //HEADER
    client.print(F("POST /fuelcell"));
    client.print(F("HTTP/1.1\r\n"));
    client.print(F("Cache-Control: no-cache\r\n"));
    client.print(F("Host: joljak-back.hsoo.co.kr"));
    client.print(F("User-Agent: Arduino\r\n"));
    client.print(F("Content-Type: application/json;charset=UTF-8\r\n"));
    client.print(F("Content-Length: "));
    client.println(jsondata.length());
    client.println();
    //BODY
    client.println(jsondata);
    client.print(F("\r\n\r\n"));
  }
}

void loop()
{
  // if there are incoming bytes available
  // from the server, read them and print them
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if the server's disconnected, stop the client
  if (!client.connected()) {
    Serial.println();
    Serial.println("Disconnecting from server...");
    client.stop();

    // do nothing forevermore
    while (true);
  }
}


void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
