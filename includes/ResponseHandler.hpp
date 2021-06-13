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

#define NOT_FOUND 404
#define SERVER_ERR 500
#define FORBIDDEN 403
#define METHOD_NOT_ALLOWED 405
#define ISFILE 1
#define ISDIR 2
#define HEAD_METHOD 3

/* 파싱된 HTTP Request를 받아 데이터를 Response 형식에 맞게 처리해주는 클래스 */
class ResponseHandler
{
	public:
		// 생성자 & 소멸자 & 대입연산자 오버로딩
		ResponseHandler(Request &request, Server &server);
		~ResponseHandler(void);
		ResponseHandler & operator=( ResponseHandler const & rhs );

		// 데이터를 실질적으로 처리하고 Response를 생성하는 함수
		Response makeResponse(void);


		/*----------ResponseHeader.cpp------------*/

		// 헤더 추가 함수
		void addResponseHeader(std::string, std::string);
		// tm 구조체를 규격에 맞는 string으로 반환해주는 함수
		std::string getFormatTime(const struct tm*);

		// 특정 헤더 추가 함수
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
		//기본 생성자 및 복사생성자
		ResponseHandler(void);
		ResponseHandler(const ResponseHandler & src);

		Request request;
		Server server;
		Location *location;
		std::string resourcePath;
		std::map<std::string, std::string> responseHeader;
		std::map<std::string, std::string> mimeType;

		//각 메소드별 Response 생성함수
		void makeTraceResponse(void);
		void makeOptionResponse(void);
		void makeGetResponse(int);
		void makeHeadResponse(void);
		void makeConnectResponse(void);
		void makePutResponse(void);
		void makePostResponse(void);
		void makeDeleteResponse(void);

		//에러 Response를 던지는 함수
		void throwErrorResponse(int, std::string) throw(Response);

		//HTML페이지를 만드는 함수
		std::string makeHTMLPage(std::string);
		std::string makeAutoIndexPage(std::string);

		//경로를 확인하는 함수
		int checkPath(std::string);
};

#endif
