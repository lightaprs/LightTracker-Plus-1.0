#include <unity.h>
#include <APRS-Decoder.h>

void test1()
{
	String message = "AB1CDE-10>APRS,AB1CDE:=1234.12N/12345.12E-QTH von AB1CDE";
	APRSMessage msg;
	msg.decode(message);
	TEST_ASSERT_EQUAL_STRING("AB1CDE-10", msg.getSource().c_str());
	TEST_ASSERT_EQUAL_STRING("AB1CDE", msg.getPath().c_str());
	TEST_ASSERT_EQUAL_STRING("APRS", msg.getDestination().c_str());
	TEST_ASSERT(msg.getType() == APRSMessageType::PositionWithoutTimestamp);
	TEST_ASSERT_EQUAL_STRING("=1234.12N/12345.12E-QTH von AB1CDE", msg.getBody()->getData().c_str());
}

void test2()
{
	String message = "OE5ABC-10>APMI06,TCPIP*,qAC,T2AUSTRIA:@101733z1234.12N/12345.12E_247/000g002t043r002p002P...h83b10191WX - Linz - Nord";
	APRSMessage msg;
	msg.decode(message);
	TEST_ASSERT_EQUAL_STRING("OE5ABC-10", msg.getSource().c_str());
	TEST_ASSERT_EQUAL_STRING("TCPIP*,qAC,T2AUSTRIA", msg.getPath().c_str());
	TEST_ASSERT_EQUAL_STRING("APMI06", msg.getDestination().c_str());
	TEST_ASSERT(msg.getType() == APRSMessageType::PositionWithTimestamp);
	TEST_ASSERT_EQUAL_STRING("@101733z1234.12N/12345.12E_247/000g002t043r002p002P...h83b10191WX - Linz - Nord", msg.getBody()->getData().c_str());
}

void test3()
{
	String message = "BH4ABC-10>APET51,TCPIP*,qAC,T2SWEDEN:!1234.12N/12345.12E-PHG4450 B8/BH4ABC APRS iGate 438.650MHz 12.4V";
	APRSMessage msg;
	msg.decode(message);
	TEST_ASSERT_EQUAL_STRING("BH4ABC-10", msg.getSource().c_str());
	TEST_ASSERT_EQUAL_STRING("TCPIP*,qAC,T2SWEDEN", msg.getPath().c_str());
	TEST_ASSERT_EQUAL_STRING("APET51", msg.getDestination().c_str());
	TEST_ASSERT(msg.getType() == APRSMessageType::PositionWithoutTimestamp);
	TEST_ASSERT_EQUAL_STRING("!1234.12N/12345.12E-PHG4450 B8/BH4ABC APRS iGate 438.650MHz 12.4V", msg.getBody()->getData().c_str());
}

void test4()
{
	String message = "VK2ABC-9>APOTC1,WIDE1-1,WIDE2-1,qAR,VK2BLR:/184701h1234.12S/12345.12E>/A=001074 13.6V 27C    DD Com Nicsan";
	APRSMessage msg;
	msg.decode(message);
	TEST_ASSERT_EQUAL_STRING("VK2ABC-9", msg.getSource().c_str());
	TEST_ASSERT_EQUAL_STRING("WIDE1-1,WIDE2-1,qAR,VK2BLR", msg.getPath().c_str());
	TEST_ASSERT_EQUAL_STRING("APOTC1", msg.getDestination().c_str());
	TEST_ASSERT(msg.getType() == APRSMessageType::PositionWithTimestamp);
	TEST_ASSERT_EQUAL_STRING("/184701h1234.12S/12345.12E>/A=001074 13.6V 27C    DD Com Nicsan", msg.getBody()->getData().c_str());
}

void test5()
{
	String message = "OE5BPA-10>APRS:!1418.68N/04819.82E_.../...g...t050r...p...P...h00b......DHT22";
	APRSMessage msg;
	msg.decode(message);
	TEST_ASSERT_EQUAL_STRING("OE5BPA-10", msg.getSource().c_str());
	TEST_ASSERT_EQUAL_STRING("", msg.getPath().c_str());
	TEST_ASSERT_EQUAL_STRING("APRS", msg.getDestination().c_str());
	TEST_ASSERT(msg.getType() == APRSMessageType::PositionWithoutTimestamp);
	TEST_ASSERT_EQUAL_STRING("!1418.68N/04819.82E_.../...g...t050r...p...P...h00b......DHT22", msg.getBody()->getData().c_str());
}

void copy_test()
{
	String message = "VK2ABC-9>APOTC1,WIDE1-1,WIDE2-1,qAR,VK2BLR:/184701h1234.12S/12345.12E>/A=001074 13.6V 27C    DD Com Nicsan";
	APRSMessage msg_old;
	msg_old.decode(message);
	APRSMessage msg(msg_old);
	TEST_ASSERT_EQUAL_STRING("VK2ABC-9", msg.getSource().c_str());
	TEST_ASSERT_EQUAL_STRING("WIDE1-1,WIDE2-1,qAR,VK2BLR", msg.getPath().c_str());
	TEST_ASSERT_EQUAL_STRING("APOTC1", msg.getDestination().c_str());
	TEST_ASSERT(msg.getType() == APRSMessageType::PositionWithTimestamp);
	TEST_ASSERT_EQUAL_STRING("/184701h1234.12S/12345.12E>/A=001074 13.6V 27C    DD Com Nicsan", msg.getBody()->getData().c_str());
}

void assignment_test()
{
	String message = "VK2ABC-9>APOTC1,WIDE1-1,WIDE2-1,qAR,VK2BLR:/184701h1234.12S/12345.12E>/A=001074 13.6V 27C    DD Com Nicsan";
	APRSMessage msg_old;
	msg_old.decode(message);
	APRSMessage msg = msg_old;
	TEST_ASSERT_EQUAL_STRING("VK2ABC-9", msg.getSource().c_str());
	TEST_ASSERT_EQUAL_STRING("WIDE1-1,WIDE2-1,qAR,VK2BLR", msg.getPath().c_str());
	TEST_ASSERT_EQUAL_STRING("APOTC1", msg.getDestination().c_str());
	TEST_ASSERT(msg.getType() == APRSMessageType::PositionWithTimestamp);
	TEST_ASSERT_EQUAL_STRING("/184701h1234.12S/12345.12E>/A=001074 13.6V 27C    DD Com Nicsan", msg.getBody()->getData().c_str());
}

#ifdef NATIVE
int main(int argc, char** argv)
#else
void setup()
#endif
{
	UNITY_BEGIN();
	RUN_TEST(test1);
	RUN_TEST(test2);
	RUN_TEST(test3);
	RUN_TEST(test4);
	RUN_TEST(test5);
	RUN_TEST(copy_test);
	RUN_TEST(assignment_test);
	UNITY_END();
}

void loop()
{
}
