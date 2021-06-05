#ifndef RESPONSEHANDLER_HPP
# define RESPONSEHANDLER_HPP

# include "webserv.h"
# include "Response.hpp"
# include "Request.hpp"
# include "Location.hpp"
# include "Server.hpp"

class Response;
class Server;
class Location;

class ResponseHandler
{
	public:
		ResponseHandler(Request &Req, std::vector<Server>::iterator server);
		ResponseHandler(const ResponseHandler & src);
		~ResponseHandler(void);
		ResponseHandler & operator=( ResponseHandler const & rhs );

		Response makeResponse(void);

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
		void addLastModifiedHeader(std::string);
		void addAllowHeader(std::string);
		void addHostHeader(void);


		/*---------------------------------------*/


	private:
		ResponseHandler(void);
		Request _Req;
		std::vector<Server>::iterator _server;
		Location *_location;
		std::string _resourcePath;
		std::map<std::string, std::string> _responseHeader;
		std::map<std::string, std::string> _mimeType;

		void _makeTraceResponse(void);
		void _makeOptionResponse(void);
		void _makeGetResponse(int);
		void _makeConnectResponse(void);
		void _makePutResponse(void);
		void _makeDeleteResponse(void);


		void _throwErrorResponse(int httpStatus, std::string version) throw(Response);

		std::string _makeHTMLPage(std::string);
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