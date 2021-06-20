#ifndef IOOBJECT_HPP
# define IOOBJECT_HPP

# include "webserv.h"

// 클라이언트의 상태를 나타내는 enum
enum Status
{
	REQUEST_RECEIVING,	// 리퀘스트를 수신중
	RESPONSE_READY		// 리스폰스를 줄 준비가 됨
};

// 오브젝트들의 타입을 지정
enum Type
{
	NONE,				// 기본 값. 해당 값이면 에러
	SERVER,				// 서버 타입
	CLIENT,				// 클라이언트 타입
	RESOURCE			// 리소스 타입
};

/*
 * Server, Client, Resource, CgiResource들이 상속 받는 부모 클래스
 * 다형성을 통해 읽기, 쓰기 작업을 통일한다.
 */
class IoObject
{
	protected:
		int				fd;		// 파일 또는 소켓 fd
		std::string		buffer;	// 다 못 읽었을 경우 데이터를 들고 있을 버퍼
		Status			status;	// 현재 리스폰스를 줄 수 있는지 상태를 저장
		Type			type;	// server, client, resource 중에 어떤 객체인지 지정
	public:
		IoObject();
		IoObject(Type type);
		IoObject(const IoObject &other);
		IoObject(int fd, std::string buffer, Status status, Type type);
		virtual ~IoObject();

		IoObject &operator=(IoObject const &other);

		// getters
		int			getFd();
		const std::string	&getBuffer();
		Status		getStatus();
		Type		getType();

		// setters
		void		setFd(int fd);
		void		setBuffer(std::string buffer);
		void		setStatus(Status status);

		// inheriting functions
		virtual void doRead() = 0;		// 하위 클래스가 읽기 작업을 하게 함
		virtual void doWrite() = 0;		// 하위 클래스가 쓰기 작업을 하게 함
};
#endif
