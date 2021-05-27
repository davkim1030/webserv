#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <string>
# include "webserv.h"


class Request
{
	public:

		Request();
		Request( std::string const );
		Request( Request const & src );
		~Request();

		Request &		operator=( Request const & rhs );

		static const std::string HEADER_LIST[];

	private:
		std::string	rawRequest;

		std::string	method;
		std::string	uri;
		std::string	httpVersion;

		std::string rawHeader;
		std::map<std::string, std::string> header;

		std::string	rawBody;

		void	parseRequest(void);
		void	setRawRequest( std::string );
};



#endif /* ********************************************************* REQUEST_H */