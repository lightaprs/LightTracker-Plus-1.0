#include <unity.h>
#include <APRS-Decoder.h>

void base91_test1()
{
	String message = "OE3MZC-9>APMZC3,WIDE1-1,WIDE2-2,qAR,OE3XFW-10:!/6.j!RQ20>mTG/A=001301 14.0V 34C";
	APRSMessage msg;
	msg.decode(message);
	TEST_ASSERT_EQUAL_STRING("OE3MZC-9", msg.getSource().c_str());
	TEST_ASSERT_EQUAL_STRING("WIDE1-1,WIDE2-2,qAR,OE3XFW-10", msg.getPath().c_str());
	TEST_ASSERT_EQUAL_STRING("APMZC3", msg.getDestination().c_str());
	TEST_ASSERT(msg.getType() == APRSMessageType::PositionWithoutTimestamp);
	TEST_ASSERT_EQUAL_STRING("!/6.j!RQ20>mTG/A=001301 14.0V 34C", msg.getBody()->getData().c_str());
	// Latitude	48.1564739608218
	// Longitude	15.9647228070544
	// Symbol	>
	// Symbol table	/
	// Speed	91.9587 km/h
	// Course	304°
	// Altitude	396.545 m
	// Comment	14.0V 34C
}

void base91_test2()
{
	String message = "OE3MZC-9>APRS,qAS,OE3XOC-11:!/6,f?RPGM>G!QLoRa 18dBmW SNR=+10dB RSSI=-88db";
	APRSMessage msg;
	msg.decode(message);
	TEST_ASSERT_EQUAL_STRING("OE3MZC-9", msg.getSource().c_str());
	TEST_ASSERT_EQUAL_STRING("qAS,OE3XOC-11", msg.getPath().c_str());
	TEST_ASSERT_EQUAL_STRING("APRS", msg.getDestination().c_str());
	TEST_ASSERT(msg.getType() == APRSMessageType::PositionWithoutTimestamp);
	TEST_ASSERT_EQUAL_STRING("!/6,f?RPGM>G!QLoRa 18dBmW SNR=+10dB RSSI=-88db", msg.getBody()->getData().c_str());
	// Latitude	48.2008290324105
	// Longitude	15.9314302515449
	// Symbol	>
	// Symbol table	/
	// Altitude	305.208 m
	// Comment	LoRa 18dBmW SNR=+10dB RSSI=-88db
}

void mice_test1()
{
	String message = "OE3SZA-9>TW5Y24,OE3XXR,WIDE1*,WIDE2-1,qAR,OE3SZA-10:`*Hjn{A>/`\"7X}438.900MHz Toff -760_%";
	APRSMessage msg;
	msg.decode(message);
	TEST_ASSERT_EQUAL_STRING("OE3SZA-9", msg.getSource().c_str());
	TEST_ASSERT_EQUAL_STRING("OE3XXR,WIDE1*,WIDE2-1,qAR,OE3SZA-10", msg.getPath().c_str());
	TEST_ASSERT_EQUAL_STRING("TW5Y24", msg.getDestination().c_str());
	TEST_ASSERT(msg.getType() == APRSMessageType::CurrentMicEData);
	TEST_ASSERT_EQUAL_STRING("`*Hjn{A>/`\"7X}438.900MHz Toff -760_%", msg.getBody()->getData().c_str());
	// Latitude	47.9873333333333
	// Longitude	14.7463333333333
	// Symbol	>
	// Symbol table	/
	// Speed	53.708 km/h
	// Course	137°
	// Altitude	338 m
	// Comment	438.900MHz Toff -760
}

void mice_test2()
{
	String message = "OE5LFM-9>TXRT50,WIDE1-1,WIDE2-1,qAU,OE5XDO-10:`*(n!@,>/]\"3r}145.750MHz T123 -060Mandi=";
	APRSMessage msg;
	msg.decode(message);
	TEST_ASSERT_EQUAL_STRING("OE5LFM-9", msg.getSource().c_str());
	TEST_ASSERT_EQUAL_STRING("WIDE1-1,WIDE2-1,qAU,OE5XDO-10", msg.getPath().c_str());
	TEST_ASSERT_EQUAL_STRING("TXRT50", msg.getDestination().c_str());
	TEST_ASSERT(msg.getType() == APRSMessageType::CurrentMicEData);
	TEST_ASSERT_EQUAL_STRING("`*(n!@,>/]\"3r}145.750MHz T123 -060Mandi=", msg.getBody()->getData().c_str());
	// Latitude	48.4723333333333
	// Longitude	14.2956666666667
	// Symbol	>
	// Symbol table	/
	// Speed	42.596 km/h
	// Course	157°
	// Altitude	736 m
	// Comment	145.750MHz T123 -060Mandi
}

#ifdef NATIVE
int main(int argc, char** argv)
#else
void setup()
#endif
{
	UNITY_BEGIN();
	RUN_TEST(base91_test1);
	RUN_TEST(base91_test2);
	RUN_TEST(mice_test1);
	RUN_TEST(mice_test2);
	UNITY_END();
}

void loop()
{
}
