#ifndef CGI_HPP
#define CGI_HPP

#include "webserv.h"
#include "IoObject.hpp"

class Cgi : public IoObject
{

		// int				fd;		// 파일 또는 소켓 fd
		// std::string		buffer;	// 다 못 읽었을 경우 데이터를 들고 있을 버퍼
		// Status			status;	// 현재 리스폰스를 줄 수 있는지 상태를 저장
		// Type			type;	// server, client, resource 중에 어떤 객체인지 지정
	private:
		pid_t pid;

	public:
		Cgi();
		Cgi(int fd, pid_t pid);
		Cgi(const Cgi &cgi);
		Cgi &operator=(const Cgi &cgi);
		~Cgi();

		IoObject *clone();


};


#endif