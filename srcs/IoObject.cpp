#include "IoObject.hpp"

IoObject::IoObject()
: fd(-1), buffer(), status(REQUEST_RECEIVING_HEADER), type(NONE)
{
}

IoObject::IoObject(Type type)
: fd(-1), buffer(), status(REQUEST_RECEIVING_HEADER), type(type)
{
}

IoObject::IoObject(int fd, std::string buffer, Status status, Type type)
: fd(fd), buffer(buffer), status(status), type(type)
{
}

IoObject::IoObject(const IoObject &other)
: fd(other.fd), buffer(other.buffer), status(other.status)
{
}

IoObject &IoObject::operator=(IoObject const &other)
{
    if (this != &other)
    {
        this->fd = other.fd;
        this->buffer = other.buffer;
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

const std::string &IoObject::getBuffer()
{
    return (buffer);
}

Status  IoObject::getStatus()
{
    return (status);
}

Type  IoObject::getType()
{
    return (type);
}

std::string &IoObject::getTempBuffer()
{
    return (tempBuffer);
}

// setters

void IoObject::setFd(int fd)
{
    this->fd = fd;
}

void IoObject::setBuffer(std::string buffer)
{
    this->buffer = buffer;
}

void IoObject::setStatus(Status status)
{
    this->status = status;
}

void IoObject::setTempBuffer(std::string buffer)
{
    this->tempBuffer = buffer;
}