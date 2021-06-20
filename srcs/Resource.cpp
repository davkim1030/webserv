#include "IoObject.hpp"
#include "Resource.hpp"
#include "Socket.hpp"

/*
 * 디폴트 생성자
 * fd=-1, buffer="", status=REQUEST_RECEIVING_HEADER, type=RESOURECE로 셋팅
 */
Resource::Resource()
: IoObject(-1, "", REQUEST_RECEIVING_HEADER, RESOURCE), ioStatus(PROCESSING)
{
}

/*
 * 복사 생성자
 */
Resource::Resource(const Resource &other)
: IoObject(other.fd, other.buffer, other.status, RESOURCE), ioStatus(other.ioStatus)
{
}

Resource::Resource(int fd)
: IoObject(fd, "", REQUEST_RECEIVING_HEADER, RESOURCE), ioStatus(PROCESSING)
{
}

/*
 * private 디폴트 생성자를 제외한 기본 생성자
 * @param int fd:	읽거나 쓸 정적 파일의 fd
 * @param const std::string buffer: 읽기/쓰기 도중 남은 데이터 저장할 공간
 * @param Status status: 현재 상태를 나타냄
 */
Resource::Resource(int fd, const std::string &buffer, Status status, IoStatus ioStatus)
: IoObject(fd, buffer, status, RESOURCE), ioStatus(ioStatus)
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
		buffer = other.buffer;
		status = other.status;
		type = RESOURCE;
		ioStatus = other.ioStatus;
	}
	return (*this);
}

/*
 * 소멸자
 */
Resource::~Resource()
{
}

/*
 * 읽기 동작을 수행하는 멤버 함수
 * @std::string &content: 읽은 파일 내용
 * @return int:
 * 		-1 : 에러 발생
 * 		0 : 모두 읽음
 * 		1 : 읽는 중
 */
/*
 읽는 중 -> content에 값을 반환하지 않음
 다 읽음 -> content에 값을 반환
*/
int		Resource::doRead(std::string &content)
{
	try
	{
		if (Socket::getInstance()->getPool()[fd] == NULL)
		{
			Socket::getInstance()->getPool()[fd] = new Resource(*this);
			content = "";
			ioStatus = PROCESSING;
			return (1);
		}
		content = buffer;
		buffer = "";
	}
	catch(const std::exception& e)
	{
		return (-1);
	}
	return (ioStatus == DONE ? 0 : 1);
}

/*
 * 쓰기 동작을 수행하는 멤버 함수
 * @param const std::string &content: 쓰기 동작을 수행할 내용
 * @return int:
 * 		-1 : 에러 발생
 * 		0 : 모두 씀
 * 		1 : 쓰는 중
 */
int		Resource::doWrite(const std::string &content)
{
	try
	{
		if (Socket::getInstance()->getPool()[fd] == NULL)
		{
			Socket::getInstance()->getPool()[fd] = new Resource(*this);
			buffer = content;
			ioStatus = PROCESSING;
			return (1);
		}
		buffer += content;
	}
	catch(const std::exception& e)
	{
		return (-1);
	}
	return (ioStatus == DONE ? 0 : 1);
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
