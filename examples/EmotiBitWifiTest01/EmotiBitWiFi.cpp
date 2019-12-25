#include "EmotiBitWiFi.h"

int8_t EmotiBitWiFi::setup()
{
#if defined(ADAFRUIT_FEATHER_M0)
	WiFi.setPins(8, 7, 4, 2);
#endif

	if (WiFi.status() == WL_NO_SHIELD) {
		return WL_NO_SHIELD;
	}

	WiFi.lowPowerMode();

	return SUCCESS;
}

int8_t EmotiBitWiFi::begin(const char* ssid, const char* pass)
{
	int8_t status;
	uint16_t timeout = 10000;
	uint32_t now = millis();

	//status = setup();
	//if (setup() != SUCCESS)
	//{
	//	return status;
	//}

	while (status != WL_CONNECTED) 
	{
		if (millis() - now > timeout)
		{
			return status;
		}
		Serial.print("Attempting to connect to SSID: ");
		Serial.println(ssid);
		// ToDo: Add WEP support
		status = WiFi.begin(ssid, pass);
		delay(1000);
	}
	WiFi.setTimeout(15);
	Serial.println("Connected to WiFi");

	Serial.print("Starting EmotiBit advertising connection on port ");
	Serial.println(_advertisingPort);
	_advertisingCxn.begin(_advertisingPort);

	return SUCCESS;
}

//
//int8_t EmotiBitWiFi::update()
//{
//	int8_t status = SUCCESS;
//	if (status == SUCCESS) {
//		status = processAdvertising();
//	}
//	if (status == SUCCESS) {
//		status == processControl();
//	}
//	return status;
//}

int8_t EmotiBitWiFi::processAdvertising()
{
	EmotiBitPacket::Header outPacketHeader;
	String outMessage;

	// ToDo: Consider need for a while loop here handle packet backlog
	int packetSize = _advertisingCxn.parsePacket();
	if (packetSize) { 
		outMessage = "";

		// read the packet into packetBufffer
		int len = _advertisingCxn.read(_inPacketBuffer, EmotiBitPacket::MAX_TO_EMOTIBIT_PACKET_LEN);
		if (len > 0) {
			_inPacketBuffer[len] = '\0';
			_receivedAdvertisingMessage = String(_inPacketBuffer);
			Serial.print(_receivedAdvertisingMessage);
			int16_t dataStartChar = EmotiBitPacket::getHeader(_receivedAdvertisingMessage, _packetHeader);
			if (dataStartChar != 0) {
				bool sendMessage = false;
				IPAddress senderIp;
				uint16_t senderPort;
				if (_packetHeader.typeTag.equals(EmotiBitPacket::TypeTag::HELLO_EMOTIBIT))
				{
					senderIp = _advertisingCxn.remoteIP();
					senderPort = _advertisingCxn.remotePort();

					outPacketHeader.timestamp = millis();
					outPacketHeader.packetNumber = advertisingPacketCounter++;
					outPacketHeader.dataLength = 0;
					outPacketHeader.typeTag = String(EmotiBitPacket::TypeTag::HELLO_HOST);
					outPacketHeader.protocolVersion;
					outPacketHeader.dataReliability = 1;
					outMessage += EmotiBitPacket::headerToString(outPacketHeader);
					if (_controlCxn.connected())
					{
						outMessage += "," + _controlPort;
					} 
					else 
					{
						outMessage += ",-1";
					}
					outMessage += "\n";
					sendMessage = true;
				}
				else if (_packetHeader.typeTag.equals(EmotiBitPacket::TypeTag::EMOTIBIT_CONNECT))
				{
					if (!_controlCxn.connected()) 
					{
						// if we aren't already connected to a computer
						_remoteIp = _advertisingCxn.remoteIP();

						// ToDo: parse message data to obtain correct ports
						_controlPort = 11999;
						_dataPort = 3001;

						Serial.print("\nStarting control connection to server: ");
						Serial.print(_remoteIp);
						Serial.print(" : ");
						Serial.print(_controlPort);
						Serial.print(" ... ");
						if (_controlCxn.connect(_remoteIp, _controlPort))
						{
							_isConnected = true;
							_controlCxn.flush();
							Serial.println("connected");
              
              // ToDo: Send a message to host to confirm connection

							Serial.print("Starting data connection to server: ");
							Serial.print(_remoteIp);
							Serial.print(" : ");
							Serial.println(_dataPort);
							_dataCxn.begin(_dataPort);
						}
					}
				}
				if (sendMessage)
				{
					sendAdvertising(outMessage, senderIp, senderPort);
				}
			}
		}

	}
	return SUCCESS;
}

int8_t EmotiBitWiFi::sendAdvertising(const String& message, const IPAddress& ip, const uint16_t& port)
{
	sendUdp(_advertisingCxn, message, ip, port);
}

int8_t EmotiBitWiFi::sendData(String & message)
{
	if (_isConnected)
	{
		sendUdp(_dataCxn, message, _remoteIp, _dataPort);
	}
}

int8_t EmotiBitWiFi::sendUdp(WiFiUDP& udp, const String& message, const IPAddress& ip, const uint16_t& port)
{
	static int16_t firstIndex;
	firstIndex = 0;
	while (firstIndex < message.length()) {
		static int16_t lastIndex;
		lastIndex = message.length();
		while (lastIndex - firstIndex > MAX_SEND_LEN) {
			lastIndex = message.lastIndexOf(EmotiBitPacket::PACKET_DELIMITER_CSV, lastIndex - 1);
			//Serial.println(lastIndex);
		}
		//Serial.println(outputMessage.substring(firstIndex, lastIndex));
		for (uint8_t n = 0; n < _nUdpSends; n++) {
			udp.beginPacket(ip, port);
			udp.print(message.substring(firstIndex, lastIndex));
			udp.endPacket();
		}
		firstIndex = lastIndex + 1;	// increment substring indexes for breaking up sends
	}
}

uint8_t EmotiBitWiFi::readControl(String& packet) {
	uint8_t numPackets = 0;
	if (_controlCxn.connected()) {
		while (_controlCxn.available()) {
			char c = _controlCxn.read();
			_receivedControlMessage += c;
			if (c == EmotiBitPacket::PACKET_DELIMITER_CSV) {
				numPackets++;
				break;
			}
		}
	}
}
