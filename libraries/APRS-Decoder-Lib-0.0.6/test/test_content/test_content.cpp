#include <unity.h>
#include <APRS-Decoder.h>
#include <iostream>

void APRSMessage_Test1()
{
	APRSMessageType type;
	type = APRSMessageType::Status;
	TEST_ASSERT(type == APRSMessageType::Status);
	TEST_ASSERT_EQUAL_STRING("Status", type.toString().c_str());
}

void APRSMessage_Test2()
{
	APRSMessageType type('@');
	type = APRSMessageType('?');
	TEST_ASSERT(type == APRSMessageType::Query);
	TEST_ASSERT_EQUAL_STRING("Query", type.toString().c_str());
}

void APRSMessage_Test3()
{
	APRSMessageType type('@');
	TEST_ASSERT_TRUE(type);
}

void APRSMessage_Test4()
{
	APRSMessageType type('U');
	TEST_ASSERT_FALSE(type);
}

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
	std::cout << msg.toString().c_str() << std::endl;
}

#ifdef NATIVE
int main(int argc, char** argv)
#else
void setup()
#endif
{
	UNITY_BEGIN();
	RUN_TEST(APRSMessage_Test1);
	RUN_TEST(APRSMessage_Test2);
	RUN_TEST(APRSMessage_Test3);
	RUN_TEST(APRSMessage_Test4);
	RUN_TEST(test1);
	UNITY_END();
}

void loop()
{
}
