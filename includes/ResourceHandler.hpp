#ifndef RESOURCEHANDLER_HPP
#define RESOURCEHANDLER_HPP

#include "Resource.hpp"
#include "webserv.h"
#include "Location.hpp"
#include "Request.hpp"
#include "ResponseMaker.hpp"
#include "ResponseHandler.hpp"
#include "Request.hpp"
#include "Socket.hpp"
#include "Server.hpp"

#define CHECK_SUCCES -1

/*

*/
class ResourceHandler : public ResponseMaker
{
	private:
		std::string resourcePath;	// 처리할 리소스의 위치
		int fd;						// 리소스의 fd
		struct stat	sb;				// fstat으로 fd를 판단할 때 매개변수로 필요한 buffer
		int clientFd;				// 리소스를 호출한 크라이언트의 fd
		int exist;					//resourcePath의 존재여부를 기록하는 int

		int tryToOpen(int);
		std::string parseResourcePath(std::string); 
		int checkGetMethodIndex(void);
		int tryToRead();
		int tryToPost();
		int tryToPut();

		ResourceHandler();

	public:
		//tmpClient->getRequest().getUri()
		ResourceHandler(const Request&, const Server&, const Location&, int clientFd);
		ResourceHandler(const ResourceHandler&);
		ResourceHandler &operator=(const ResourceHandler&);
		~ResourceHandler();

		void setReadFlag();
		void setWriteFlag();
		bool checkAllowMethod(void);
		bool CheckResourceType(void);
		int wasExist(void);
};

#endif
