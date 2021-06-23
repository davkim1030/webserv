#ifndef RESPONSEHANDLER_HPP
#define RESPONSEHANDLER_HPP

#include "Resource.hpp"
#include "webserv.h"
#include "Location.hpp"
#include "Request.hpp"
#include "ResponseMaker.hpp"
#include "Socket.hpp"

#define NOT_FOUND 404
#define SERVER_ERR 500
#define FORBIDDEN 403
#define METHOD_NOT_ALLOWED 405
#define NOT_EXIST 0
#define ISFILE 1
#define ISDIR 2
#define HEAD_METHOD 3
#define CHECK_SUCCES -1

/*
이 객체의 역할
---클라이언트 READ---
0. allow-method check
1. resource-path 생성
2. resource를 open
3. resource를 읽고/쓸 수 있는지 유효성 검사
---리소스 READ---
1. fd를 fd풀에 등록 후 '읽기' fd set 플래그 설정
2. fd를 fd풀에 등록 후 '쓰기' fd set.
	- 유효성 검사 결과가 엉망일 경우 resource->status에 에러. resource->type에 상태코드를 넘겨준다.
	- 이후 바깥에서는 NormalResponse를 만들어서 읽기/쓰기 된 resource에 대해서 처리한다.
3. NormalResponse를 받아서 해당 string을 '쓰기' fd set.
4. fd 내용이 다 읽힌 게 확인되면, fd를 pool에서 삭제
*/
class ResponseHandler 
{
	private:
		ResponseHandler();
		Request request;
		Location location;
		std::string resourcePath;
		int fd;
		struct stat	sb;
		int clientFd;

		int tryToOpen(int);
		std::string parseResourcePath(std::string); 

	public:
		//tmpClient->getRequest().getUri()
		ResponseHandler(const Request&, const Location&, int clientFd);
		ResponseHandler(const ResponseHandler&);
		ResponseHandler &operator=(const ResponseHandler&);
		~ResponseHandler();

		int tryToRead();
		int tryToWrite();
		void setReadFlag();
		void setWriteFlag();
		int checkPath(std::string path);
		int checkAllowMethod(void);
		int checkGetMethodIndex(void);
};

#endif
