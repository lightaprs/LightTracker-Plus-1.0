#include "APRSPosition.h"
/*
APRSPosition::APRSPosition()
	: APRSHeader(APRSMessageTypes::PositionWithoutTimestamp)
{
}

APRSPosition::~APRSPosition()
{
}

double APRSPosition::getLatitude() const
{
	return _lat;
}

void APRSPosition::setLatitude(const double lat)
{
	_lat = lat;
}

double APRSPosition::getLongitude() const
{
	return _long;
}

void APRSPosition::setLongitude(const double longi)
{
	_long = longi;
}

APRSPositionTypes APRSPosition::getPositionType() const
{
	return _type;
}

bool APRSPosition::decode(const String & message)
{

}

String APRSPosition::toString() const
{
	return APRSHeader::toString() + ", Latitude: " + _lat + ", Longitude: " + _long;
}



APRSPositionWithTimestamp::APRSPositionWithTimestamp()
	: APRSPosition(APRSMessageTypes::PositionWithTimestamp)
{
}

APRSPositionWithTimestamp::~APRSPositionWithTimestamp()
{
}

int APRSPositionWithTimestamp::getTime() const
{
	return _time;
}

void APRSPositionWithTimestamp::setTime(const int time)
{
	_time = time;
}

bool APRSPositionWithTimestamp::decode(const String & message)
{

}

String APRSPositionWithTimestamp::toString() const
{
	return APRSPosition::toString() + ", Time: " + _time;
}
*/
