#include "webserv.h"
# include "ResponseHandler.hpp"

/*
* 응답 헤더에 키와 키에 따른 밸류를 할당합니다.
* @param 새로이 추가될 헤더의 키, 헤더의 밸류
*/
void ResponseHandler::addResponseHeader(std::string key, std::string value)
{
	this->responseHeader[key] = value;
}

/*
* 시간값을 포맷에 맞게 변환합니다.
* @param 변환할 tm 구조체
* @return 변환한 char *
*/
std::string ResponseHandler::getFormatTime(const struct tm* timeinfo)
{
	char buffer[80];

	strftime(buffer, 80, "%a, %d, %b %Y %X GMT", timeinfo);
	return (std::string(buffer));
}

/*
* Date 헤더를 추가합니다.
*/
void ResponseHandler::addDateHeader(void)
{
	time_t rawtime;
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	addResponseHeader("Date", getFormatTime(timeinfo));
}

/*
* 서버 헤더를 추가합니다. 'FDB'
*/
void ResponseHandler::addServerHeader(void)
{
	addResponseHeader("Server", "FDB");
}

/*
* 파일의 확장자에 대응하는 mime-type을 content-type 헤더로 추가해주는 함수
* @param 타입을 구할 파일의 경로
*/
void ResponseHandler::addContentTypeHeader(std::string extension)
{
	if (mimeType.count(extension) == 0)
		addResponseHeader("Content-Type", "text/plain");
	else
		addResponseHeader("Content-Type", mimeType[extension]);
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
		throw Response(500, responseHeader, makeHTMLPage("500"), request.getHttpVersion());
	timeinfo = localtime(&statbuff.st_mtime);
	addResponseHeader("Last-Modified", getFormatTime(timeinfo));
}

void ResponseHandler::addContentLengthHeader(int length)
{
	char *lenStr = ft_itoa(length);
	addResponseHeader("Content-Length", static_cast<std::string>(lenStr));
	free(lenStr);
}

void ResponseHandler::addContentLanguageHeader()
{
	addResponseHeader("Content-Language", "ko-KR");
}

void ResponseHandler::addContentLocationHeader()
{
	addResponseHeader("Content-Location", this->resourcePath.substr(1));
}

void ResponseHandler::addAllowHeader(std::string allow)
{
	addResponseHeader("Allow", allow);
}

void ResponseHandler::addHostHeader()
{
	addResponseHeader("Host", resourcePath);
}
