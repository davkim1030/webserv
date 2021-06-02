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

		//Request getRequest(void) const;
		//int getStatusCode(void) const;
		//std::string getBody(void) const;
		//std::string getVersion(void) const;
		//std::map<std::string, std::string> getResponseHeader(void) const;

		//void setRequest(Request);
		//void setStatusCode(int);
		//void setBody(std::string);
		//void setVersion(std::string);
		//void setResponseHeader(std::map<std::string, std::string>);

		void makeResponse(void);

		/*----------ResponseHeader.cpp------------*/

		void addResponseHeader(std::string, std::string);

		void addDateHeader(void);
		void addServerHeader(void);
		void addAllowHeader(void);
		void addContentLanguageHeader(void);
		void addContentLengthHeader(void);
		void addContentLocationHeader(void);
		void addContentTypeHeader(void);
		void addLastModifiedHeader(void);
		/*---------------------------------------*/


	private:
		ResponseHandler(void);
		Request _Req;

		//int _statusCode;
		//std::string _body;
		//std::string _version;
		std::map<std::string, std::string> _responseHeader;


		void _makeGetResponse(int);
		std::string makeErrorPage(int);
		enum statusCode { NOT_FOUND, SERVER_ERR, FORBIDDEN, HEAD_METHOD};

};

#endif