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

	std::vector<Server>::iterator iter;
	for (iter = ServerConfig::getInstance()->getServers().begin();
		iter != ServerConfig::getInstance()->getServers().end(); iter++)
	{
		std::string key = iter->getIp() + ":" + ft_itoa(iter->getPort());
		if (tmp.find(key) != tmp.end())
		{
			// tmp 맵에 있는 이전 내용을 새로운 내용으로 덮어 씌우는 부분
			continue ;
		}
		struct sockaddr_in	serverAddr;
		iter->setSocketFd(socket(PF_INET, SOCK_STREAM, 0));		// 소켓 fd 생성
		ft_memset(&serverAddr, '\0', sizeof(serverAddr));		// serverAddr 초기화
		serverAddr.sin_family = AF_INET;						// IPv4 설정
		serverAddr.sin_addr.s_addr = inet_addr(iter->getIp().c_str());	// ip 주소 설정
		serverAddr.sin_port = htons(iter->getPort());			// 포트 설정

		// 소켓, fd 바인드
		if (bind(iter->getSocketFd(), (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
			throw BindException();
		// 데이터를 받을 서버 소켓 열기
		if (listen(iter->getSocketFd(), socketCnt) == -1)
			throw ListenException();
		
		FD_SET(iter->getSocketFd(), &rfds);
		FD_SET(iter->getSocketFd(), &wfds);
		FD_SET(iter->getSocketFd(), &efds);

		// Socket에서 관리할 서버 리스트에 위에서 생성한 서버 추가
		servers[iter->getSocketFd()][iter->getServerName()] = *iter;
		// 중복 방지를 위해 tmp map에 추가
		tmp[key] = iter->getSocketFd();

		// select()로 감시할 fd의 최대값 업데이트
		if (fdMax < iter->getSocketFd())
			fdMax = iter->getSocketFd();
	}
	return (true);
}

bool Socket::runServer(struct timeval timeout, unsigned int bufferSize)
{
	fd_set	cpyRfds;
	fd_set	cpyWfds;
	fd_set	cpyEfds;

	struct sockaddr_in	clientAddr;
	socklen_t			addrSize = sizeof(clientAddr);
	int					fdNum;
	char				buf[bufferSize];

	while (1)
	{
		usleep(5);	// cpu 100% 점유 방지

		// fds들의 데이터 유실 방지를 위해 복사
		cpyRfds = rfds;
		cpyWfds = wfds;
		cpyEfds = efds;

		// fd_set 변수의 0 ~ fdMax + 1까지 비트를 감시하여 읽기, 쓰기, 에러 요구가 일어났는지 확인
		if ((fdNum = select(fdMax + 1, &cpyRfds, &cpyWfds, &cpyEfds, &timeout)) == -1)
			// TODO: throw proper error
		if (fdNum == 0) // 처리할 요구가 없으면 다시 위로
			continue ;

		// fd를 0부터 fdMax - 1까지 반복하며 set된 플래그 값이 있는지 확인하여 처리		
		for (int i = 0; i < fdMax; i++)
		{
			if (FD_ISSET(i, &cpyRfds))	// read 요청이 온 경우
			{
				if (servers.count(i) == 1)	// 서버 소켓의 경우
				{
					int clientSocket = accept(i, (struct sockaddr *)&clientAddr, &addrSize);
					if (clientSocket == -1)
						throw AcceptException();
					fcntl(clientSocket, F_SETFL, O_NONBLOCK);	// 해당 클라이언트 fd를 논블록으로 변경
					FD_SET(clientSocket, &wfds);
					FD_SET(clientSocket, &rfds);
					FD_SET(clientSocket, &efds);
					if (fdMax < clientSocket)
						fdMax = clientSocket;
					
					clients[clientSocket].setServerSocketFd(i);
					clients[clientSocket].setSocketFd(clientSocket);
					clients[clientSocket].setLastReqMs();	// TODO: ms 단위 현재 시간 가져오는 함수 만들기
				}
				else						// 클라이언트 소켓
				{
					int		len;
					bool	isReadable = false;

					clients[i].setLastReqMs();		// TODO: ms 단위 현재 시간 가져오는 함수 만들기
					while ((len = read(i, buf, bufferSize)) > 0)
					{
						isReadable = true;
						buf[len] = 0;
						clients[i].getRawRequest() += buf;
					}
					if (clients[i].getStatus() == REQUEST_RECEIVING && clients[i].getRequest)
				}
			}
		}
	}
	timeout.tv_sec += 1;
	bufferSize = bufferSize + 1;
	return false;
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
