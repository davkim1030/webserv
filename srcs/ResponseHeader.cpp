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
	strftime(buffer, 80, "%a, %d, %b %Y %X GMT", timeinfo);
	addResponseHeader("Date", std::string(buffer));
}

void ResponseHandler::addServerHeader(void)
{
	this->_responseHeader["server"] = "FDB";
}

void ResponseHandler::addLastModifiedHeader(void)
{

}

//mime-type과 들어온 확장자를 대조해서 헤더를 추가해주는 함수
void ResponseHandler::addContentTypeHeader(std::string extension)
{
	size_t i = extension.find_first_of('.');
	std::string filePath = std::string(extension, i + 1, extension.size() - i);

	if (i == std::string::npos || _mimeType.count(filePath) == 0)
		addResponseHeader("Content-Type: ", _mimeType["text/plain"]);
	else
		addResponseHeader("Content-Type: ", _mimeType[filePath]);
}