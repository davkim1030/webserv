#ifndef IOOBJECT_HPP
# define IOOBJECT_HPP

# include "webserv.h"

// 클라이언트의 상태를 나타내는 enum
enum Status
{
	REQUEST_RECEIVING_HEADER,	// 헤더 리퀘스트를 수신중
	REQUEST_RECEIVING_BODY,		// 바디 리퀘스트를 수신중
	RESPONSE_READY,				// 리스폰스를 줄 준비가 됨
	PROCESSING_ERROR			// 처리 도중 에러를 만남
};

// 오브젝트들의 타입을 지정
enum Type
{
	NONE,				// 기본 값. 해당 값이면 에러
	SERVER,				// 서버 타입
	CLIENT,				// 클라이언트 타입
	RESOURCE,			// 리소스 타입
	CGI,				// CGI 타입
	CGI_RESOURCE		// CGI 리소스
};

/*
 * Server, Client, Resource, CgiResource들이 상속 받는 부모 클래스
 * 다형성을 통해 읽기, 쓰기 작업을 통일한다.
 */
class IoObject
{
	protected:
		int				fd;		// 파일 또는 소켓 fd
		std::string		buffer;	// 읽거나 쓸 데이터를 담아둔 버퍼
		Status			status;	// 현재 리스폰스를 줄 수 있는지 상태를 저장
		Type			type;	// 객체의 타입을 지정

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
		virtual IoObject *clone() = 0;
};
#endif
