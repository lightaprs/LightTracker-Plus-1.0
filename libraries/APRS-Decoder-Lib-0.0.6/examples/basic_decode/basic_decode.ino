#include <Arduino.h>
#include <APRS-Decoder.h>

void setup()
{
	String message[] = {
		"AB1CDE-10>APRS,AB1CDE:=1234.12N/12345.12E-QTH von AB1CDE",
		"OE5ABC-10>APMI06,TCPIP*,qAC,T2AUSTRIA:@101733z1234.12N/12345.12E_247/000g002t043r002p002P...h83b10191WX - Linz - Nord",
		"BH4ABC-10>APET51,TCPIP*,qAC,T2SWEDEN:!1234.12N/12345.12E-PHG4450 B8/BH4ABC APRS iGate 438.650MHz 12.4V",
		"VK2ABC-9>APOTC1,WIDE1-1,WIDE2-1,qAR,VK2BLR:/184701h1234.12S/12345.12E>/A=001074 13.6V 27C    DD Com Nicsan",
		"OE5BPA-10>APRS:!1418.68N/04819.82E_.../...g...t050r...p...P...h00b......DHT22"
	};
	size_t n = sizeof(message) / sizeof(message[0]);
	for(size_t i = 0; i < n; i++)
	{
		APRSMessage msg;
		msg.decode(message[i]);
		Serial.println(msg.toString());
	}
}

void loop()
{

}
