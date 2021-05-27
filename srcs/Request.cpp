
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

void Request::initRequest(void)
{
	this->method.clear();
	this->uri.clear();
	this->httpVersion.clear();

	this->rawHeader.clear();
	this->header.clear();

	this->rawBody.clear();
}

void Request::parseRequest(void)
{
	this->method = parseMethod();
	this->uri = parseUri();
	this->httpVersion = parseHttpVersion();

	this->rawHeader = this->rawRequest.substr(this->rawRequest.find("\r\n") + 1, this->rawRequest.find("\r\n\r\n"));
	this->rawBody = this->rawRequest.substr(this->rawRequest.find("\r\n\r\n") + 1, this->rawRequest.length());

	this->header = parseHeader();
}

std::string Request::parseMethod(void) {
	this->method = this->rawRequest.substr(0, rawRequest.find(' '));
}

std::string Request::parseUri(void) {
	std::string firstLine = this->rawRequest.substr(0, this->rawRequest.find("\r\n"));
	std::size_t start = firstLine.find(' ');
	std::size_t end = firstLine.find_last_of(' ');
	this->uri = firstLine.substr(start + 1, end);
}

std::string Request::parseHttpVersion(void) {
	std::string firstLine = this->rawRequest.substr(0, this->rawRequest.find("\r\n"));
	this->httpVersion = firstLine.substr(firstLine.find_last_of(' ') + 1);
}

std::map<std::string, std::string> Request::parseHeader(void)
{
	std::string rawHeader = this->getRawHeader();
	std::string key;
	size_t keyEndPos;
	size_t keyStartPos = 0;

	while (keyEndPos = rawHeader.find(": ") != std::string::npos && keyStartPos != std::string::npos)
	{
		key = rawHeader.substr(keyStartPos, keyEndPos);
		if (keyStartPos = rawHeader.find("\r\n"))
			break ;
		this->header[key] = rawHeader.substr(keyEndPos + 2, keyStartPos);
	}
}

void Request::setRawRequest(std::string data){	this->rawRequest = data;	}
std::string Request::getMethod(void) const {	return this->method;	}
std::string Request::getUri(void) const {	return this->uri;	}
std::string Request::getHttpVersion(void) const {	return this->httpVersion;	}
std::string Request::getRawHeader(void) const {	return this->rawHeader;	}
std::map<std::string, std::string> Request::getHeader(void) const {	return this->header;	}
std::string Request::getRawBody(void) const {	return this->rawBody;	}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/



/* ************************************************************************** */