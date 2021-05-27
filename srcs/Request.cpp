
#include "../includes/Request.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/


Request::Request()
{
	this->rawRequest.clear();

	this->method.clear();
	this->uri.clear();
	this->httpVersion.clear();

	this->rawHeader.clear();
	this->header.clear();

	this->rawBody.clear();
}

Request::Request(std::string const data)
{
	setRawRequest(data);

	this->method.clear();
	this->uri.clear();
	this->httpVersion.clear();

	this->rawHeader.clear();
	this->header.clear();

	this->rawBody.clear();
}

Request::Request( const Request & src ) :
{
	setRawRequest(src.rawHeader);

	this->method.clear();
	this->uri.clear();
	this->httpVersion.clear();

	this->rawHeader.clear();
	this->header.clear();

	this->rawBody.clear();

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
	//if ( this != &rhs )
	//{
		//this->_value = rhs.getValue();
	//}
	return *this;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void Request::parseRequest(void)
{
	this->rawHeader = this->rawRequest.substr(this->rawRequest.find("\r\n") + 1, this->rawRequest.find("\r\n\r\n"));

	std::string key = "Content-Length: ";
	std::size_t found = this->rawHeader.find(key);

	if (found != std::string::npos)
	{
		std::size_t target_pos = found + key.length();

		this->content_length = this->rawHeader.substr(target_pos, this->rawHeader.find("\r\n", target_pos) - target_pos);
	}

}

void Request::setRawRequest(std::string data)
{
	this->rawRequest = data;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

const std::string Request::HEADER_LIST[] = {
		"Accept-Charsets",
		"Accept-Language",
		"Allow",
		"Authorization",
		"Content-Language",
		"Content-Length",
		"Content-Location",
		"Content-Type",
		"Date",
		"Host",
		"Last-Modified",
		"Location",
		"Referer",
		"Retry-After",
		"Server",
		"Transfer-Encoding",
		"User-Agent",
		"WWW-Authenticate"
	};


/* ************************************************************************** */