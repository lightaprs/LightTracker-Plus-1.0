#ifndef APRS_HEADER_H_
#define APRS_HEADER_H_

#include <Arduino.h>
#include "APRSMessageType.h"

class APRSBody
{
public:
	APRSBody();
	virtual ~APRSBody();

	String getData() const;
	void setData(const String & data);

	virtual bool decode(const String & message);
	virtual String encode() const;
	virtual String toString() const;

private:
	String _data;
};

class APRSMessage
{
public:
	APRSMessage();
	APRSMessage(APRSMessage & other_msg);
	APRSMessage & operator=(APRSMessage & other_msg);
	virtual ~APRSMessage();

	String getSource() const;
	void setSource(const String & source);

	String getDestination() const;
	void setDestination(const String & destination);

	String getPath() const;
	void setPath(const String & path);

	APRSMessageType getType() const;

	String getRawBody() const;
	APRSBody * const getBody();

	virtual bool decode(const String & message);
	virtual String encode() const;
	virtual String toString() const;

private:
	String _source;
	String _destination;
	String _path;
	APRSMessageType _type;
	String _rawBody;
	APRSBody * const _body;
};


#endif
