
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

	std::string key = "Content-Length: ";
	std::size_t found = this->raw_header.find(key);

	if (found != std::string::npos)
	{
		std::size_t target_pos = found + key.length();

		this->content_length = this->raw_header.substr(target_pos, this->raw_header.find("\r\n", target_pos) - target_pos);
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