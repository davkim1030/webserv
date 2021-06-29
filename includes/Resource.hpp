#ifndef RESOURCE_HPP
# define RESOURCE_HPP

# include "webserv.h"
# include "IoObject.hpp"

// 리소스 파일의 읽기 쓰기 상태
enum IoStatus
{
	PROCESSING,	// 읽기 / 쓰기 동작중
	DONE,		// 동작 완료
	ERROR		// 에러 발생
};

/*
 * 정적 파일의 읽기, 쓰기를 처리하는 클래스
 */
class Resource : public IoObject
{
	private:
		Resource();
		IoStatus	ioStatus;	// 읽기 쓰기 상태
		int			clientFd;	// 리소스를 호출한 클라이언트의 fd
		int			pos;		// 읽기 쓰기 중인 커서 위치

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
