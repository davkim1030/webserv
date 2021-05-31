#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "webserv.h"

class Request
{
	public:

		Request();
		Request( std::string const );
		Request( Request const & src );
		~Request();

		Request &		operator=( Request const & rhs );

		std::string getMethod(void) const;
		std::string getUri(void) const;
		std::string getHttpVersion(void) const;
		std::string getRawHeader(void) const;
		std::map<std::string, std::string> getHeader(void) const;
		std::string getRawBody(void) const;

		void	initRequest(void);
		void	setRawRequest(std::string);
		void	parseRequest(void);

	private:
		std::string	rawRequest;

		std::string	method;
		std::string	uri;
		std::string	httpVersion;

		std::string rawHeader;
		std::map<std::string, std::string> header;

		std::string	rawBody;

		std::string parseMethod(void);
		std::string parseUri(void);
		std::string parseHttpVersion(void);
		std::map<std::string, std::string> parseHeader(std::string);
};


#endif /* ********************************************************* REQUEST_H */
