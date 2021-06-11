#include "Socket.hpp"

void	printFds(fd_set rfds, fd_set wfds, fd_set efds, int fdMax)
{
	std::cout << "rfds : ";
	for (int i = 0; i <= fdMax; i++)
	{
		std::cout << (FD_ISSET(i, &rfds) ? 1 : 0) << " ";
	}
	std::cout << std::endl;

	std::cout << "wfds : ";
	for (int i = 0; i <= fdMax; i++)
	{
		std::cout << (FD_ISSET(i, &wfds) ? 1 : 0) << " ";
	}
	std::cout << std::endl;

	std::cout << "efds : ";
	for (int i = 0; i <= fdMax; i++)
	{
		std::cout << (FD_ISSET(i, &efds) ? 1 : 0) << " ";
	}
	std::cout << std::endl;
	std::cout << "@@@@@@@@@@@@@@@@@@@@@@" << std::endl;
	usleep(100 * 1000);
}

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
 * select 함수로 fd_set 선택시 에러
 */
const char *Socket::SelectException::what() const throw()
{
	return ("Error: select failed");
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
		std::cout << "IP: " << iter->getIp() << "$" << std::endl;
		std::cout << "ServerName: " << iter->getServerName() << "$" << std::endl;
		std::cout << "port: " << iter->getPort() << "$" << std::endl;
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
		
		std::cout << iter->getIp() << ":" << iter->getPort() << std::endl;
		
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

	int		fdNum;

	unsigned long		timeoutMs = timeout.tv_sec * 1000 + timeout.tv_usec / 1000;

	while (1)
	{
		usleep(5);	// cpu 100% 점유 방지

		// fds들의 데이터 유실 방지를 위해 복사
		cpyRfds = rfds;
		cpyWfds = wfds;
		cpyEfds = efds;

		// fd_set 변수의 0 ~ fdMax + 1까지 비트를 감시하여 읽기, 쓰기, 에러 요구가 일어났는지 확인
		printFds(rfds, wfds, efds, fdMax);
		if ((fdNum = select(fdMax + 1, &cpyRfds, &cpyWfds, &cpyEfds, &timeout)) == -1)
			throw SelectException();
		if (fdNum == 0) // 처리할 요구가 없으면 다시 위로
			continue ;
		
		// fd를 0부터 fdMax - 1까지 반복하며 set된 플래그 값이 있는지 확인하여 처리		
		for (int i = 0; i < fdMax + 1; i++)
		{
			if (FD_ISSET(i, &cpyRfds))	// read 요청이 온 경우
			{
				if (servers.find(i) != servers.end())	// 서버 소켓의 경우
				{
					struct sockaddr_in	clientAddr;
					socklen_t	addrSize = sizeof(clientAddr);

					ft_memset(&clientAddr, '\0', addrSize);
					int clientSocket = accept(i, (struct sockaddr *)&clientAddr, &addrSize);
					if (clientSocket == -1)
					{
						std::cout << "ERROR FD " << i << std::endl;
						for (std::map<int, std::map<std::string, Server> >::iterator iter = servers.begin();
								iter != servers.end(); iter++)
							std::cout << iter->first <<": " << iter->second.size() << std::endl;
						std::cout << strerror(errno) << std::endl;
						throw AcceptException();
					}
					std::cout << "Open client socket: " << clientSocket << std::endl;
					std::cout << servers[i]["default_server"].getPort() << std::endl;
					fcntl(clientSocket, F_SETFL, O_NONBLOCK);	// 해당 클라이언트 fd를 논블록으로 변경
					FD_SET(clientSocket, &rfds);
					FD_SET(clientSocket, &wfds);
					FD_SET(clientSocket, &efds);
					if (fdMax < clientSocket)
						fdMax = clientSocket;
					
					clients[clientSocket].setServerSocketFd(i);
					clients[clientSocket].setSocketFd(clientSocket);
					clients[clientSocket].setLastReqMs(ft_get_time());
				}
				else						// 클라이언트 소켓
				{
					int		len;
					bool	isReadable = false;
					char	buf[bufferSize];

					clients[i].setLastReqMs(ft_get_time());
					// TODO: 한 번에 한 번씩 읽어야만 나중에 문제 안 생김 !수정 필요!
					while ((len = read(i, buf, bufferSize)) > 0)
					{
						isReadable = true;
						buf[len] = 0;
						clients[i].getRequest().setRawRequest(
							clients[i].getRequest().getRawRequest() + buf
						);
					}
					if (clients[i].getStatus() == REQUEST_RECEIVING && clients[i].getRequest().isParsable())
					{
						clients[i].getRequest().initRequest();
						clients[i].getRequest().parseRequest();
						// Request::host 처리하는 부분 구현 필요
						std::string serverName = clients[i].getRequest().getHost();
						size_t	idx;
						if ((idx = serverName.find(':') ) != std::string::npos)
							serverName = serverName.substr(0, idx);
						// TODO default server 수정 해야됨
						clients[i].setResponse(ResponseHandler(clients[i].getRequest(), servers[3]["default_server"]).makeResponse());
						clients[i].getRequest().initRequest();
						clients[i].setStatus(RESPONSE_READY);
					}
					if (isReadable == false)
					{
						clearConnectedSocket(i);
						if (len == 0)
							std::cout << "Disconnected " << i << "in Server" << std::endl;
						else
							std::cout << "Error occured client " << i << "in Server" << std::endl;
					}
				}
			}
			else if (FD_ISSET(i, &cpyWfds))	// write 요청이 온 경우
			{
				// 항상 클라이언트 소켓만 들어옴
				if (ft_get_time() - clients[i].getLastReqMs() > timeoutMs)
				{
					clearConnectedSocket(i);
					continue ;
				}
				if (clients[i].getStatus() == RESPONSE_READY)
				{
					write(i, clients[i].getResponse().getMessage().c_str(), clients[i].getResponse().getMessage().size());
					if (clients[i].getResponse().getLastResponse() == 401)
						clearConnectedSocket(i);
					else
					{
						clients[i].getResponse();
						clients[i].setStatus(REQUEST_RECEIVING);
						clearConnectedSocket(i);
					}
				}
				FD_CLR(i, &wfds);
			}
			else if (FD_ISSET(i, &cpyEfds))	// except 요청이 온 경우
			{
				if (servers.count(i) == 1)
				{
					FD_CLR(i, &rfds);
					FD_CLR(i, &efds);
					close(i);
					servers.erase(servers.find(i));
				}
				else
				{
					clearConnectedSocket(i);
				}
			}
		}
	}
	return (true);
}

/*
 * 연결된 클라이언트의 fd를 fd_set들에서 clear하고 연결을 close 함
 * @param int socketFd: 연결을 종료할 클라이언트 소켓 fd
 */
void	Socket::clearConnectedSocket(int fd)
{
	FD_CLR(fd, &rfds);
	FD_CLR(fd, &wfds);
	FD_CLR(fd, &efds);

	close(fd);
	clients.erase(clients.find(fd));
	if (fd == fdMax)
		fdMax--;
	std::cout << "Connection close fd: " << fd << std::endl;
}
