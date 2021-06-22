#ifndef CGIRESOURCE_HPP
#define CGIRESOURCE_HPP

#include "webserv.h"
#include "IoObject.hpp"

class CgiResource : public IoObject
{

		// int				fd;		// 파일 또는 소켓 fd
		// std::string		buffer;	// 다 못 읽었을 경우 데이터를 들고 있을 버퍼
		// Status			status;	// 현재 리스폰스를 줄 수 있는지 상태를 저장
		// Type			type;	// server, client, resource 중에 어떤 객체인지 지정
	private:
		pid_t	pid;
		bool	lseekFlag;
		int		clientFd;
		int		fileSize;

	public:
		CgiResource();
		CgiResource(int fd, pid_t pid, int clientFd);
		CgiResource(const CgiResource &cgi);
		CgiResource &operator=(const CgiResource &cgi);
		~CgiResource();

		IoObject	*clone();
		
		pid_t		getPid();
		int			getClientFd();
		int			getFileSize();

		bool		isLseek();

		void		setLseek();
		void		setFileSize(int fileSize);
};

#endif
