#ifndef RESOURCE_HPP
# define RESOURCE_HPP

# include "webserv.h"
# include "IoObject.hpp"

enum IoStatus
{
	PROCESSING,
	DONE,
	ERROR
};

/*
 * 정적 파일의 읽기, 쓰기를 처리하는 클래스
 */
class Resource : public IoObject
{
	private:
		Resource();
		IoStatus	ioStatus;
		int			clientFd;
		int			pos;
	public:
		Resource(const Resource &other);
		Resource(int fd, int clientFd);
		Resource(int fd, const std::string &buffer, Status status, IoStatus ioStatus, int clientFd);
		~Resource();
		Resource &operator=(const Resource &other);
		
		void	setIoStatus(IoStatus ioStatus);
		void	setPos(int pos);

		IoStatus	getIoStatus();
		int			getClientFd();
		int			getPos();
		
		IoObject *clone();
};
#endif
