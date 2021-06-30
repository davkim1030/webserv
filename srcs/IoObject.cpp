#include "IoObject.hpp"

IoObject::IoObject()
: fd(-1), readBuffer(), writeBuffer(), status(REQUEST_RECEIVING_HEADER), type(NONE)
{
}

IoObject::IoObject(Type type)
: fd(-1), readBuffer(), writeBuffer(), status(REQUEST_RECEIVING_HEADER), type(type)
{
}

IoObject::IoObject(int fd, std::string readBuffer, Status status, Type type)
: fd(fd), readBuffer(readBuffer), writeBuffer(), status(status), type(type)
{
}

IoObject::IoObject(const IoObject &other)
: fd(other.fd), readBuffer(other.readBuffer), writeBuffer(other.writeBuffer), status(other.status)
{
}

IoObject &IoObject::operator=(IoObject const &other)
{
	if (this != &other)
	{
		this->fd = other.fd;
		this->readBuffer = other.readBuffer;
		this->writeBuffer = other.writeBuffer;
		this->status = other.status;
		this->type = other.type;
	}
	return (*this);
}

IoObject::~IoObject()
{
}

// getters

int IoObject::getFd()
{
	return (fd);
}

const std::string &IoObject::getReadBuffer()
{
	return (readBuffer);
}

const std::string &IoObject::getWriteBuffer()
{
	return (writeBuffer);
}

Status  IoObject::getStatus()
{
	return (status);
}

Type  IoObject::getType()
{
	return (type);
}

// setters

void	IoObject::setFd(int fd)
{
	this->fd = fd;
}

void	IoObject::setWriteBuffer(std::string writeBuffer)
{
	this->writeBuffer = writeBuffer;
}

void	IoObject::setReadBuffer(std::string readBuffer)
{
	this->readBuffer = readBuffer;
}

void	IoObject::setStatus(Status status)
{
	this->status = status;
}
