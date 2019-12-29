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
String dataMessage;
const uint16_t DATA_SEND_INTERVAL = 100;
const uint16_t DATA_MESSAGE_RESERVE_SIZE = 4096;



void setup() 
{
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
	uint32_t serialTimer = millis();
  while (!Serial) 
	{
		// wait for serial port to connect. Needed for native USB port only
		if (millis() - serialTimer > 5000)
		{
			break; 
		}
  }

	dataMessage.reserve(DATA_MESSAGE_RESERVE_SIZE);

	emotibitWifi.setup();
	emotibitWifi.begin(ssid, pass);
	printWiFiStatus();
}

void loop() { 
	emotibitWifi.update(dataMessage);
	Serial.print(dataMessage);

  if (emotibitWifi._isConnected) {
		// Read control packets
		String controlPacket;
		while (emotibitWifi.readControl(controlPacket))
		{
			// ToDo: handling some packets (e.g. disconnect behind the scenes)
			Serial.print("Receiving control msg: ");
			Serial.println(controlPacket);
			EmotiBitPacket::Header header;
			EmotiBitPacket::getHeader(controlPacket, header);
			if (header.typeTag.equals(EmotiBitPacket::TypeTag::EMOTIBIT_DISCONNECT))
			{
				emotibitWifi.disconnect();
			}
			dataMessage += controlPacket;
		}

	  // Send data periodically
	  static uint32_t dataSendTimer = millis();
	  if (millis() - dataSendTimer > DATA_SEND_INTERVAL) {
			dataSendTimer = millis();
			String data = "0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9";
			static uint16_t counter = 0;
			for (int i = 0; i < 20; i++)
			{
				dataMessage += EmotiBitPacket::createPacket(EmotiBitPacket::TypeTag::DEBUG, emotibitWifi.dataPacketCounter++, data, 50);
			}

			emotibitWifi.sendData(dataMessage);
			dataMessage = "";
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
