#include "webserv.h"
#include "Client.hpp"

/*
 * 클라이언트 클래스 기본 생성자
 * fd 값들을 -1로 지정하여 초기화 하지 않으면 사용 못 하게 함
 */
Client::Client() : status(REQUEST_RECEIVING), serverSocketFd(-1), socketFd(-1),
		response(200, std::map<std::string, std::string>(), "")
{
}

/*
 * 멤버 값들을 명시적으로 지정해주는 생성자
 * @param: int serverSocketFd: 클라이언트가 연결된 서버의 소켓 fd
 * @param: int socketFd: 클라이언트에 할당된 소켓 fd
 */
Client::Client(int serverSocketFd, int socketFd)
	: serverSocketFd(serverSocketFd), socketFd(socketFd),
	response(200, std::map<std::string, std::string>(), "")
{
	status = REQUEST_RECEIVING;
}

Client::~Client()
{
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

int		Client::getSocketFd()
{
	return (socketFd);
}

int		Client::getServerSocketFd()
{
	return (serverSocketFd);
}

Request	&Client::getRequest()
{
	return (request);
}

Response	&Client::getResponse()
{
	return (response);
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
