#ifndef RESPONSEHANDLER_HPP
# define RESPONSEHANDLER_HPP

# include "webserv.h"

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

		void addDateHeader(void);
		void addServerHeader(void);
		void addAllowHeader(void);
		void addContentLanguageHeader(void);
		void addContentLengthHeader(void);
		void addContentLocationHeader(void);
		void addContentTypeHeader(std::string);
		void addLastModifiedHeader(void);

		/*---------------------------------------*/


	private:
		ResponseHandler(void);
		Request _Req;
		std::string _resourcePath;
		std::map<std::string, std::string> _responseHeader;
		std::map<std::string, std::string> _mimeType;

		void _makeGetResponse(int);

		std::string _makeErrorPage(int);
		std::string _makeAutoIndexPage(std::string);
		int _checkPath(std::string);


		enum statusCode { NOT_FOUND, SERVER_ERR, FORBIDDEN, HEAD_METHOD};
		#define ISFILE 1
		#define ISDIR 2

};

#endif