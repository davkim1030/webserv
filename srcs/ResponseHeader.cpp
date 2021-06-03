#include "webserv.h"

void ResponseHandler::addResponseHeader(std::string key, std::string value)
{
	this->_responseHeader[key] = value;
}

const char* ResponseHandler::_getFormatTime(const struct tm* timeinfo)
{
	char buffer[80];

	strftime(buffer, 80, "%a, %d, %b %Y %X GMT", timeinfo);
	return (buffer);
}

void ResponseHandler::addDateHeader(void)
{
	time_t rawtime;
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	const char *buffer = _getFormatTime(timeinfo);
	addResponseHeader("Date", std::string(buffer));
}

void ResponseHandler::addServerHeader(void)
{
	this->_responseHeader["server"] = "FDB";
}

/*
* 파일의 확장자에 대응하는 mime-type을 content-type 헤더로 추가해주는 함수
* @param 타입을 구할 파일의 경로
*/
void ResponseHandler::addContentTypeHeader(std::string extension)
{
	size_t i = extension.find_first_of('.');
	std::string filePath = std::string(extension, i + 1, extension.size() - i);

	if (i == std::string::npos || _mimeType.count(filePath) == 0)
		addResponseHeader("Content-Type: ", _mimeType["text/plain"]);
	else
		addResponseHeader("Content-Type: ", _mimeType[filePath]);
}

/*
* 경로의 최신 수정시간을 헤더에 추가해주는 함수
* @param 최신 수정시간을 구할 경로
*/ 
void ResponseHandler::addLastModifiedHeader(std::string path)
{
	struct stat statbuff;
	struct tm*	timeinfo;

	if (stat(path.c_str(), &statbuff) < 0)
		throw Response(500, _responseHeader, _makeErrorPage(500), _Req.getHttpVersion());
	timeinfo = localtime(&statbuff.st_mtime);
	const char *buffer = _getFormatTime(timeinfo);
	addResponseHeader("Last-Modified: ", (std::string)buffer);
}