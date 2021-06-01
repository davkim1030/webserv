#include "webserv.h"

void ResponseHandler::addResponseHeader(std::string key, std::string value)
{
	this->_responseHeader[key] = value;
}

void ResponseHandler::addDateHeader(void)
{	
	time_t rawtime;
	char buffer[80];
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	//Mon, 18 Jul 2016 16:06:00 GMT
	strftime(buffer, 80, "%a, %d, %b %Y %X GMT", timeinfo);
	addResponseHeader("Date", std::string(buffer));
}

void addServerHeader(void)
{
	
}

void addLastModifiedHeader(void)
{
	
}
