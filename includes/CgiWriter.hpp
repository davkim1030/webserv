#ifndef CGIWRITER_HPP
#define CGIWRITER_HPP

#include "webserv.h"
#include "IoObject.hpp"

class CgiWriter : public IoObject
{

		// int				fd;		// 파일 또는 소켓 fd
		// std::string		buffer;	// 다 못 읽었을 경우 데이터를 들고 있을 버퍼
		// Status			status;	// 현재 리스폰스를 줄 수 있는지 상태를 저장
		// Type			type;	// server, client, resource 중에 어떤 객체인지 지정
	private:
		int		clientFd;
		int		pos;

	public:
		CgiWriter();
		CgiWriter(int fd, int clientFd);
		CgiWriter(const CgiWriter &cgi);
		CgiWriter &operator=(const CgiWriter &cgi);
		~CgiWriter();

		IoObject	*clone();
		int			getClientFd();
		int			getPos();

		void		setPos(int pos);

};


#endif