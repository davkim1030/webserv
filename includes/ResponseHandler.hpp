#ifndef RESPONSEHANDLER_HPP
# define RESPONSEHANDLER_HPP

# include "webserv.h"
# include "Response.hpp"
# include "Request.hpp"

class Response;

class ResponseHandler
{
	public:
		ResponseHandler(Request &Req);
		ResponseHandler(const ResponseHandler & src);
		~ResponseHandler(void);
		ResponseHandler & operator=( ResponseHandler const & rhs );

		void makeResponse(void);

		/*----------ResponseHeader.cpp------------*/

		void addResponseHeader(std::string, std::string);
		std::string _getFormatTime(const struct tm*);

		void addDateHeader(void);
		void addServerHeader(void);
		void addAllowHeader(void);
		void addContentLanguageHeader(void);
		void addContentLengthHeader(int);
		void addContentLocationHeader(void);
		void addContentTypeHeader(std::string);
		void addLastModifiedHeader(std::string path);

		/*---------------------------------------*/


	private:
		ResponseHandler(void);
		Request _Req;
		std::string _resourcePath;
		std::map<std::string, std::string> _responseHeader;
		std::map<std::string, std::string> _mimeType;

		void _makeGetResponse(int);

		void _throwErrorResponse(int httpStatus, std::string version) throw(Response);

		std::string _makeErrorPage(int);
		std::string _makeAutoIndexPage(std::string);
		int _checkPath(std::string);

		#define NOT_FOUND 404
		#define SERVER_ERR 500
		#define FORBIDDEN 403
		#define ISFILE 1
		#define ISDIR 2
		#define HEAD_METHOD 3
};

#endif