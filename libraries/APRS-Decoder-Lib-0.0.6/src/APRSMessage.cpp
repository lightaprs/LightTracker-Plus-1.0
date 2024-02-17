#include "APRSMessage.h"

APRSMessage::APRSMessage()
	: _body(new APRSBody())
{
}

APRSMessage::APRSMessage(APRSMessage & other_msg)
	: _source(other_msg.getSource()), _destination(other_msg.getDestination()), _path(other_msg.getPath()),
	  _type(other_msg.getType()), _rawBody(other_msg.getRawBody()), _body(new APRSBody())
{
	_body->setData(other_msg.getBody()->getData());
}

APRSMessage & APRSMessage::operator=(APRSMessage & other_msg)
{
	if(this != &other_msg)
	{
		_source = other_msg.getSource();
		_destination = other_msg.getDestination();
		_path = other_msg.getPath();
		_type = other_msg.getType();
		_rawBody = other_msg.getRawBody();
		_body->setData(other_msg.getBody()->getData());
	}
	return *this;
}

APRSMessage::~APRSMessage()
{
	delete _body;
}

String APRSMessage::getSource() const
{
	return _source;
}

void APRSMessage::setSource(const String & source)
{
	_source = source;
}

String APRSMessage::getDestination() const
{
	return _destination;
}

void APRSMessage::setDestination(const String & destination)
{
	_destination = destination;
}

String APRSMessage::getPath() const
{
	return _path;
}

void APRSMessage::setPath(const String & path)
{
	_path = path;
}

APRSMessageType APRSMessage::getType() const
{
	return _type;
}

String APRSMessage::getRawBody() const
{
	return _rawBody;
}

APRSBody * const APRSMessage::getBody()
{
	return _body;
}

bool APRSMessage::decode(const String & message)
{
	int pos_src = message.indexOf('>');
	_source = message.substring(0, pos_src);
	int pos_dest = message.indexOf(',');
	int pos_path = message.indexOf(':');
	if(pos_dest != -1 && pos_dest < pos_path)
	{
		_path = message.substring(pos_dest+1, pos_path);
		_destination = message.substring(pos_src+1, pos_dest);
	}
	else
	{
		_path = "";
		_destination = message.substring(pos_src+1, pos_path);
	}
	_rawBody = message.substring(pos_path+1);
	_type = APRSMessageType(_rawBody[0]);
	_body->decode(_rawBody);
	return bool(_type);
}

String APRSMessage::encode() const
{
	String msg;
	msg = _source + ">" + _destination;
	if(!_path.isEmpty())
	{
		msg = msg + "," + _path;
	}
	msg = msg  + ":" + _body->encode();
	return msg;
}

String APRSMessage::toString() const
{
	return "Source: " + _source + ", Destination: " + _destination + ", Path: " + _path +
		", Type: " + _type.toString() + ", " + _body->toString();
}


APRSBody::APRSBody()
{
}

APRSBody::~APRSBody()
{
}

String APRSBody::getData() const
{
	return _data;
}

void APRSBody::setData(const String & data)
{
	_data = data;
}

bool APRSBody::decode(const String & message)
{
	_data = message;
	return true;
}

String APRSBody::encode() const
{
	return _data;
}

String APRSBody::toString() const
{
	return "Data: " + _data;
}
