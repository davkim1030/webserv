#include "IoObject.hpp"
#include "Resource.hpp"
#include "Socket.hpp"

/*
 * 디폴트 생성자
 * fd=-1, buffer="", status=REQUEST_RECEIVING_HEADER, type=RESOURECE로 셋팅
 */
Resource::Resource()
: IoObject(-1, "", REQUEST_RECEIVING_HEADER, RESOURCE), ioStatus(PROCESSING), clientFd(-1), pos(0)
{
}

/*
 * 복사 생성자
 */
Resource::Resource(const Resource &other)
: IoObject(other), ioStatus(other.ioStatus), clientFd(other.clientFd), pos(other.pos)
{
}

Resource::Resource(int fd, int clientFd)
: IoObject(fd, "", REQUEST_RECEIVING_HEADER, RESOURCE), ioStatus(PROCESSING), clientFd(clientFd), pos(0)
{
}

/*
 * private 디폴트 생성자를 제외한 기본 생성자
 * @param int fd:	읽거나 쓸 정적 파일의 fd
 * @param const std::string buffer: 읽기/쓰기 도중 남은 데이터 저장할 공간
 * @param Status status: 현재 상태를 나타냄
 */
Resource::Resource(int fd, const std::string &buffer, Status status, IoStatus ioStatus, int clientFd)
: IoObject(fd, buffer, status, RESOURCE), ioStatus(ioStatus), clientFd(clientFd), pos(0)
{
}

/*
 * 할당 연산자 오버로딩
 * @param const Resource &other: 복사할 객체
 * @return Resource &: 복사받은 데이터 객체
 */
Resource &Resource::operator=(const Resource &other)
{
	if (this != &other)
	{
		fd = other.fd;
		readBuffer = other.readBuffer;
		writeBuffer = other.writeBuffer;
		status = other.status;
		type = RESOURCE;
		ioStatus = other.ioStatus;
		clientFd = other.clientFd;
		pos = other.pos;
	}
	return (*this);
}

/*
 * 소멸자
 */
Resource::~Resource()
{
}

IoObject *Resource::clone()
{
	IoObject *resource = new Resource(*this);
	return (resource);
}

void	Resource::setIoStatus(IoStatus ioStatus)
{
	this->ioStatus = ioStatus;
}

IoStatus	Resource::getIoStatus()
{
	return (ioStatus);
}

int			Resource::getClientFd()
{
	return clientFd;
}

int			Resource::getPos()
{
	return pos;
}

void		Resource::setPos(int pos)
{
	this->pos = pos;
}
