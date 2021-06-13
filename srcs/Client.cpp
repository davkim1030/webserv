#include "webserv.h"
#include "Client.hpp"

/*
 * 클라이언트 클래스 기본 생성자
 * fd 값들을 -1로 지정하여 초기화 하지 않으면 사용 못 하게 함
 */
Client::Client() : status(REQUEST_RECEIVING), serverSocketFd(-1), socketFd(-1),
		request(""), response(200, std::map<std::string, std::string>(), "")
{
	lastReqMs = ft_get_time();
}

/*
 * 복사 생성자
 * @param const Client &other: 복사할 객체
 */
Client::Client(const Client &other) : status(other.status), serverSocketFd(other.serverSocketFd),
	socketFd(other.socketFd), request(other.request), response(other.response),
	remainBody(other.remainBody), lastReqMs(other.lastReqMs)
{
}

/*
 * 멤버 값들을 명시적으로 지정해주는 생성자
 * @param: int serverSocketFd: 클라이언트가 연결된 서버의 소켓 fd
 * @param: int socketFd: 클라이언트에 할당된 소켓 fd
 */
Client::Client(int serverSocketFd, int socketFd)
	: serverSocketFd(serverSocketFd), socketFd(socketFd),
	request(""), response(200, std::map<std::string, std::string>(), "")
{
	status = REQUEST_RECEIVING;
	remainBody = 0;
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
		socketFd = other.socketFd;
		request = other.request;
		response = other.response;
		remainBody = other.remainBody;
		lastReqMs = other.lastReqMs;
	}
	return *this;
}

void	Client::setSocketFd(int socketFd)
{
	this->socketFd = socketFd;
}

void	Client::setServerSocketFd(int serverSocketFd)
{
	this->serverSocketFd = serverSocketFd;
}

void	Client::setStatus(Status status)
{
	this->status = status;
}

void	Client::setRemainBody(long long remainBody)
{
	this->remainBody = remainBody;
}

void	Client::setLastReqMs(unsigned long lastReqMs)
{
	this->lastReqMs = lastReqMs;
}

void	Client::setResponse(const Response &response)
{
	this->response = response;
}

int		Client::getSocketFd()
{
	return (socketFd);
}

int		Client::getServerSocketFd()
{
	return (serverSocketFd);
}

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

long long	Client::getRemainBody()
{
	return (remainBody);
}

unsigned long	Client::getLastReqMs()
{
	return (lastReqMs);
}