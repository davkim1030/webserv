#include "Socket.hpp"

/*
 * 기본 생성자
 * 객체의 모든 fd_set 타입 멤버 변수들의 비트 값을 모두 0으로 초기화 함
 */
Socket::Socket() : fdMax(-1)
{
	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	FD_ZERO(&efds);
}

/*
 * 소멸자
 */
Socket::~Socket()
{
}

/*
 * 소켓 생성 에외
 */
const char *Socket::SocketException::what() const throw()
{
	return ("Error: socket failed");
}

/*
 * 소켓 fd와 포트 바인딩 예외
 */
const char *Socket::BindException::what() const throw()
{
	return ("Error: bind failed");
}

/*
 * 소켓과 클라이언트 연결시 예외
 */
const char *Socket::ListenException::what() const throw()
{
	return ("Error: listen failed");
}

/*
 * 연결된 클라이언트의 데이터를 받을 시 에러
 */
const char *Socket::AcceptException::what() const throw()
{
	return ("Error: accept failed");
}

/*
 * 서버 초기 설정
 * @param int socketCnt: 
 */
bool Socket::initServer(int socketCnt)
{
	std::map<std::string, int> tmp;

	for (std::vector<Server>::iterator iter = ServerConfig::getInstance()->getServers().begin();
		iter != ServerConfig::getInstance()->getServers().end(); iter++)
	{

	}
}

bool Socket::runServer(struct timeval timeout, unsigned int bufferSize)
{
	
}

/*
 * 연결된 클라이언트의 fd를 fd_set들에서 clear하고 연결을 close 함
 * @param int socketFd: 연결을 종료할 클라이언트 소켓 fd
 */
void	Socket::clearConnectedSocket(int socketFd)
{
	FD_CLR(socketFd, &rfds);
	FD_CLR(socketFd, &wfds);
	FD_CLR(socketFd, &efds);

	close(socketFd);
	clients.erase(clients.find(socketFd));
}
