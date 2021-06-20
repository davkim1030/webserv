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
		IoStatus ioStatus;
	public:
		Resource(const Resource &other);
		Resource(int fd);
		Resource(int fd, const std::string &buffer, Status status, IoStatus ioStatus);
		~Resource();
		Resource &operator=(const Resource &other);

		int		doWrite(const std::string &content);
		int		doRead(std::string &content);
		
		void	setIoStatus(IoStatus ioStatus);

		IoStatus	getIoStatus();

		IoObject *clone();
};
#endif
