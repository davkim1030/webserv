
#include "../includes/Request.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Request::Request()
{
	this->rawRequest.clear();
	initRequest();
}

Request::Request(std::string const data)
{
	setRawRequest(data);
	initRequest();
}

Request::Request( const Request & src )
{
	setRawRequest(src.rawHeader);
	this->method = src.getMethod();
	this->uri = src.getUri();
	this->httpVersion = src.getHttpVersion();
	this->rawHeader = src.getRawHeader();
	this->header = src.getHeader();
	this->rawBody = src.getRawBody();
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Request::~Request()
{
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Request &				Request::operator=( Request const & rhs )
{
	if ( this != &rhs )
	{
		setRawRequest(rhs.rawHeader);
		this->method = rhs.getMethod();
		this->uri = rhs.getUri();
		this->httpVersion = rhs.getHttpVersion();
		this->rawHeader = rhs.getRawHeader();
		this->header = rhs.getHeader();
		this->rawBody = rhs.getRawBody();
	}
	return *this;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

/*
* 기본 생성 직후 변수들을 초기화해줍니다.
*/
void Request::initRequest(void)
{
	this->method.clear();
	this->uri.clear();
	this->httpVersion.clear();

	this->rawHeader.clear();
	this->header.clear();

	this->rawBody.clear();
}

/*
* 가공되지 않은 요청 문자열을 가져와 method, uri, httpVersion, header, body를 파싱합니다.
*/
void Request::parseRequest(void)
{
	this->method = parseMethod();
	this->uri = parseUri();
	this->httpVersion = parseHttpVersion();

	std::size_t headerStartPos = this->rawRequest.find("\r\n") + 2;
	std::size_t headerEndPos = this->rawRequest.find("\r\n\r\n");
	this->rawHeader = this->rawRequest.substr(headerStartPos, headerEndPos - headerStartPos + 2);

	this->rawBody = this->rawRequest.substr(headerEndPos + 4, this->rawRequest.length() - headerEndPos);

	this->header = parseHeader(this->getRawHeader());
}

/*
* 가공되지 않은 요청 문자열에서 method를 파싱합니다.
*/
std::string Request::parseMethod(void) {
	return (this->rawRequest.substr(0, rawRequest.find(' ')));
}

/*
* 가공되지 않은 요청 문자열에서 uri를 파싱합니다.
*/
std::string Request::parseUri(void) {
	std::string firstLine = this->rawRequest.substr(0, this->rawRequest.find("\r\n"));
	std::size_t start = firstLine.find(' ');
	std::size_t end = firstLine.find_last_of(' ');
	return (firstLine.substr(start + 1, end));
}

/*
* 가공되지 않은 요청 문자열에서 HTTP 버전을 파싱합니다.
*/
std::string Request::parseHttpVersion(void) {
	std::string firstLine = this->rawRequest.substr(0, this->rawRequest.find("\r\n"));
	return (firstLine.substr(firstLine.find_last_of(' ') + 1));
}

/*
* 가공되지 않은 요청 문자열에서 헤더를 키와 밸류값으로 나누어 파싱합니다.
*/
std::map<std::string, std::string> Request::parseHeader(std::string rawHeader)
{
	std::string key;
	std::string currHeader = rawHeader;
	size_t keyEndPos;
	size_t valueStartPos;
	size_t valueEndPos;

	std::map<std::string, std::string> header;
	while ((keyEndPos = currHeader.find(": ")) != std::string::npos)
	{
		key = currHeader.substr(0, keyEndPos);
		valueStartPos = keyEndPos + 2;
		valueEndPos = currHeader.find("\r\n");
		header[key] = currHeader.substr(valueStartPos, valueEndPos - valueStartPos);
		currHeader = currHeader.substr(valueEndPos + 2, currHeader.length() - (valueEndPos + 2));
		if ((currHeader.find("\r\n")) == std::string::npos)
			break ;
	}
	return header;
}

/*
* 가공되지 않은 요청 문자열에 인자로 받은 string을 할당합니다.
*/
void Request::setRawRequest(std::string data){	this->rawRequest = data;	}

/*
* method 값을 취합니다.
*/
std::string Request::getMethod(void) const {	return this->method;	}

/*
* uri 값을 취합니다.
*/
std::string Request::getUri(void) const {	return this->uri;	}

/*
* http 버전 값을 취합니다.
*/
std::string Request::getHttpVersion(void) const {	return this->httpVersion;	}

/*
* 가공되지 않은 헤더 데이터를 취합니다.
*/
std::string Request::getRawHeader(void) const {	return this->rawHeader;	}

/*
* 키와 밸류 값으로 나뉜 헤더 데이터를 취합니다.
*/
std::map<std::string, std::string> Request::getHeader(void) const {	return this->header; }

/*
* 가공되지 않은 바디 데이터를 취합니다.
*/
std::string Request::getRawBody(void) const {	return this->rawBody;	}
