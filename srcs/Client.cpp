#include "webserv.h"
#include "Client.hpp"
#include "Server.hpp"
#include "Socket.hpp"

/*
 * 클라이언트 클래스 기본 생성자
 * fd 값들을 -1로 지정하여 초기화 하지 않으면 사용 못 하게 함
 */
Client::Client() : IoObject(CLIENT), serverSocketFd(-1),
		request(""), response(200, std::map<std::string, std::string>(), ""), pos(0),
		tempBuffer(""), bodyLen(0), chunkedFlag(LEN)
{
	lastReqMs = ft_get_time();
}

/*
 * 복사 생성자
 * @param const Client &other: 복사할 객체
 */
Client::Client(const Client &other)
	: IoObject(other.fd, other.buffer, other.status, CLIENT),
	serverSocketFd(other.serverSocketFd), request(other.request),
	response(other.response), pos(other.pos),
	lastReqMs(other.lastReqMs), tempBuffer(other.tempBuffer), bodyLen(other.bodyLen),
	chunkedFlag(other.chunkedFlag)
{
}

/*
 * 멤버 값들을 명시적으로 지정해주는 생성자
 * @param: int serverSocketFd: 클라이언트가 연결된 서버의 소켓 fd
 * @param: int socketFd: 클라이언트에 할당된 소켓 fd
 */
Client::Client(int serverSocketFd, int fd)
	: IoObject(fd, "", REQUEST_RECEIVING_HEADER, CLIENT),
	serverSocketFd(serverSocketFd),
	request(""), response(200, std::map<std::string, std::string>(), ""), pos(0),
	tempBuffer(""), bodyLen(0), chunkedFlag(LEN)
{
	lastReqMs = ft_get_time();
}

Client::~Client()
{
}

/*
 * 할당 연산자 오버로딩
 */
Client &Client::operator=(const Client &other)
{
	if (this != &other)
	{
		status = other.status;
		serverSocketFd = other.serverSocketFd;
		fd = other.fd;
		buffer = other.buffer;
		status = other.status;
		request = other.request;
		response = other.response;
		pos = other.pos;
		lastReqMs = other.lastReqMs;
		tempBuffer = other.tempBuffer;
		bodyLen = other.bodyLen;
		chunkedFlag = other.chunkedFlag;
	}
	return *this;
}

void	Client::setServerSocketFd(int serverSocketFd)
{
	this->serverSocketFd = serverSocketFd;
}

void	Client::setStatus(Status status)
{
	this->status = status;
}

void	Client::setPos(size_t pos)
{
	this->pos = pos;
}

void	Client::setLastReqMs(unsigned long lastReqMs)
{
	this->lastReqMs = lastReqMs;
}

void	Client::setResponse(const Response &response)
{
	this->response = response;
}

void	Client::setTempBuffer(std::string buffer)
{
	this->tempBuffer = buffer;
}

void	Client::setBodyLen(size_t chunkedIndex)
{
	this->bodyLen = chunkedIndex;
}

void	Client::setChunkedFlag(ChunkedFlag chunkedFlag)
{
	this->chunkedFlag = chunkedFlag;
}

// getter

Response	&Client::getResponse()
{
	return (response);
}

Request		&Client::getRequest()
{
	return (request);
}

Status	Client::getStatus()
{
	return (status);
}

size_t	Client::getPos()
{
	return (pos);
}

unsigned long	Client::getLastReqMs()
{
	return (lastReqMs);
}

int		Client::getServerSocketFd()
{
	return (serverSocketFd);
}

std::string &Client::getTempBuffer()
{
	return (tempBuffer);
}

size_t		Client::getBodyLen()
{
	return (bodyLen);
}

ChunkedFlag	Client::getChunkedFlag()
{
	return (chunkedFlag);
}


/*
 * 현재 buffer 데이터로 HTTP Request Message의 header 부분이 추출 가능한지 확인
 * @return bool: 헤더가 다 들어왔는지
 */
bool	Client::headerParsable()
{
	return (buffer.find("\r\n\r\n") != std::string::npos);
}
IoObject *Client::clone()
{
	IoObject *tmp = new Client(*this);
	return tmp;
}

Server Client::getServer()
{
	return *dynamic_cast<Server *>(Socket::getInstance()->getPool()[serverSocketFd]);
}
