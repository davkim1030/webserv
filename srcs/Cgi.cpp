#include "Cgi.hpp"

Cgi::Cgi() : IoObject(CGI), pid(-1)
{}

Cgi::Cgi(int fd, pid_t pid) : IoObject(fd, "", REQUEST_RECEIVING_HEADER, CGI), pid(pid)
{}

Cgi::Cgi(const Cgi &cgi) : IoObject(cgi), pid(cgi.pid)
{}

Cgi &Cgi::operator=(const Cgi &cgi)
{
	if (this != &cgi)
	{
		IoObject::operator=(cgi);
		pid = cgi.pid;
	}
	return *this;
}

Cgi::~Cgi()
{}

IoObject *Cgi::clone()
{
	IoObject *tmp = new Cgi(*this);
	return tmp;
}