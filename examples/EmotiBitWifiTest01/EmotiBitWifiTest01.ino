/*
  Web client

 This sketch connects to a website (http://www.google.com)
 using a WiFi shield.

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the WiFi.begin() call accordingly.

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the WiFi.begin() call accordingly.

 Circuit:
 * WiFi shield attached

 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 31 May 2012
 by Tom Igoe
 */

//#define ARDUINO
#include "EmotiBitWiFi.h"
#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

EmotiBitWiFi emotibitWifi;
uint16_t controlPacketNumber = 0;
uint16_t dataPacketNumber = 0;

void setup() 
{
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) 
	{
    ; // wait for serial port to connect. Needed for native USB port only
  }
	emotibitWifi.setup();
	emotibitWifi.begin(ssid, pass);
	printWiFiStatus();
}

void loop() {
	emotibitWifi.processAdvertising();

	// Read control packets
	String controlPacket;
	while (emotibitWifi.readControl(controlPacket))
	{
		// ToDo: handling some packets (e.g. disconnect and ping/pong behind the scenes)
		Serial.print("Receiving control msg: ");
		Serial.println(controlPacket);
		EmotiBitPacket::Header header;
		EmotiBitPacket::getHeader(controlPacket, header);
		if (header.typeTag.equals(EmotiBitPacket::TypeTag::EMOTIBIT_DISCONNECT))
		{
			emotibitWifi.disconnect();
		}
	}

  if (emotibitWifi._isConnected) {
	  // Send data periodically
	  static uint32_t now = millis();
	  if (millis() - now > 1000) {
		  now = millis();
			Serial.print("Sending Data: ");
			static uint8_t data = 0;
			String packet = EmotiBitPacket::createPacket(EmotiBitPacket::TypeTag::DEBUG, dataPacketNumber++, String(data++), 1);
			Serial.print(packet);
			emotibitWifi.sendData(packet);
	  }
  }
}


void printWiFiStatus() {
	// print the SSID of the network you're attached to:
	Serial.print("SSID: ");
	Serial.println(WiFi.SSID());

	// print your WiFi shield's IP address:
	IPAddress ip = WiFi.localIP();
	Serial.print("IP Address: ");
	Serial.println(ip);

	// print the received signal strength:
	long rssi = WiFi.RSSI();
	Serial.print("signal strength (RSSI):");
	Serial.print(rssi);
	Serial.println(" dBm");
}
