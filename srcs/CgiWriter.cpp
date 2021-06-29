#include "CgiWriter.hpp"

CgiWriter::CgiWriter() : IoObject(CGI), clientFd(-1), pos(0)
{}

CgiWriter::CgiWriter(int fd, int clientFd)
: IoObject(fd, "", REQUEST_RECEIVING_HEADER, CGI), clientFd(clientFd), pos(0)
{}

CgiWriter::CgiWriter(const CgiWriter &cgi)
: IoObject(cgi), clientFd(cgi.clientFd), pos(cgi.pos)
{}

CgiWriter &CgiWriter::operator=(const CgiWriter &cgi)
{
	if (this != &cgi)
	{
		IoObject::operator=(cgi);
		clientFd = cgi.clientFd;
		pos = cgi.pos;
	}
	return *this;
}

CgiWriter::~CgiWriter()
{}

IoObject *CgiWriter::clone()
{
	IoObject *tmp = new CgiWriter(*this);
	return tmp;
}

int		CgiWriter::getClientFd()
{
	return this->clientFd;
}

int		CgiWriter::getPos()
{
	return this->pos;
}

void	CgiWriter::setPos(int pos)
{
	this->pos = pos;
}
