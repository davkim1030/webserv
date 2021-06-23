#ifndef RESPONSEMAKER_HPP
#define RESPONSEMAKER_HPP

#include "webserv.h"
#include "Response.hpp"
#include "Request.hpp"
#include "Location.hpp"
#include "Server.hpp"


#define NOT_FOUND 404
#define SERVER_ERR 500
#define FORBIDDEN 403
#define METHOD_NOT_ALLOWED 405
#define NOT_EXIST 0
#define ISFILE 1
#define ISDIR 2
#define HEAD_METHOD 3

class ResponseMaker
{
	protected:
		Request request;
		Server server;
		Location location;
		std::map<std::string, std::string> responseHeader;
		std::map<std::string, std::string> mimeType;

		ResponseMaker();
	public:
		ResponseMaker(const ResponseMaker&);
		ResponseMaker(const Request&, const Server&, const Location&);
		ResponseMaker &operator=(const ResponseMaker&);
		virtual ~ResponseMaker();

		void		addResponseHeader(std::string, std::string);
		std::string getFormatTime(const struct tm*);
		void		addDateHeader(void);
		void		addServerHeader(void);
		void		addContentTypeHeader(std::string);
		void		addContentLengthHeader(int);

		std::string makeHTMLPage(std::string);
		Response	makeErrorResponse(int, std::string version = "HTTP/1.1");

		int 		checkPath(std::string path);
		void		updateErrorStatus(int clientFd, int statusCode);
};


#endif