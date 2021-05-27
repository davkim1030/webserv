
#include "../includes/Request.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/




Request::Request()
{
	this->raw_request.clear();

	this->method.clear();
	this->uri.clear();
	this->http_version.clear();

	this->raw_header.clear();
	this->header.clear();

	this->raw_body.clear();

	this->status = 0;
	this->type = 0;
}

Request::Request( const Request & src )
{
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

void Request::parse_request(void)
{
	this->raw_header = this->raw_request.substr(this->raw_request.find("\r\n") + 1, this->raw_request.find("\r\n\r\n"));


}



/*
** --------------------------------- ACCESSOR ---------------------------------
*/

const std::string Request::header_list[] = {
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