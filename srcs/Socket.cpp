#include "Socket.hpp"

/*
 * 현재 read, write, except fd_set의 현황을 출력하는 함수
 * @param fd_set rfds: 읽기 fds
 * @param fd_set wfds: 쓰기 fds
 * @param fd_set efds: 오류 fds
 * @param int fdMax: 관리하는 fd의 최대 값
 */
void	printFdsStatus(fd_set rfds, fd_set wfds, fd_set efds, int fdMax)
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
 * nginx config를 파싱한 데이터를 기반으로 각 서버의 데이터를 가져와서 servers 변수에 등록
 * @param int socketCnt: config 파일에서 읽은 관리할 서버 소켓의 개수
 */
void Socket::initServer(int socketCnt)
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

		// 이전에 존재하던 key이면 내용 읽지 않고 패스
		if (tmp.find(key) != tmp.end())
			continue ;

		struct sockaddr_in	serverAddr;							// 서버 소켓의 ip주소
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
		
		// 생성한 서버 소켓 fd 값을 fd_set 변수들에 대해 설정
		FD_SET(iter->getSocketFd(), &rfds);
		FD_SET(iter->getSocketFd(), &wfds);
		FD_SET(iter->getSocketFd(), &efds);

		servers[iter->getSocketFd()] = *iter;	// Socket에서 관리할 서버 리스트에 위에서 생성한 서버 추가
		tmp[key] = iter->getSocketFd();			// 중복 방지를 위해 tmp map에 추가

		// select()로 감시할 fd의 최대값 업데이트
		if (fdMax < iter->getSocketFd())
			fdMax = iter->getSocketFd();
	}
}

/*
 * 실제 서버를 동작시키는 메인 로직
 * 꼭 initServer를 호출하여 서버 정보들 등록 후 동작 시켜야 함.
 * @param struct timeval timeout: 한 클라이언트에서 최대 대기할 시간(ms)
 * @param unsigned int bufferSize: 한 번에 읽을 데이터의 사이즈
 */
void Socket::runServer(struct timeval timeout, unsigned int bufferSize)
{
	fd_set	cpyRfds;
	fd_set	cpyWfds;
	fd_set	cpyEfds;
	int		fdNum;
	unsigned long	timeoutMs = timeout.tv_sec * 1000 + timeout.tv_usec / 1000;

	// 메인 루프
	while (1)
	{
		usleep(5);	// cpu 100% 점유 방지

		// fds들의 데이터 유실 방지를 위해 복사
		cpyRfds = rfds;
		cpyWfds = wfds;
		cpyEfds = efds;

		printFdsStatus(rfds, wfds, efds, fdMax);
		// fd_set 변수의 0 ~ fdMax + 1까지 비트를 감시하여 읽기, 쓰기, 에러 요구가 일어났는지 확인
		if ((fdNum = select(fdMax + 1, &cpyRfds, &cpyWfds, &cpyEfds, &timeout)) == -1)
			throw SelectException();
		if (fdNum == 0) // 처리할 요구가 없으면 다시 위로
			continue ;
		
		// fd를 0부터 fdMax까지 반복하며 set된 플래그 값이 있는지 확인하여 처리		
		for (int i = 0; i < fdMax + 1; i++)
		{
			if (FD_ISSET(i, &cpyRfds))	// read 요청이 온 경우
			{
				if (servers.find(i) != servers.end())	// 서버 소켓의 경우
				{
					struct sockaddr_in	clientAddr;
					socklen_t	addrSize = sizeof(clientAddr);

					// 주소 변수 데이터 초기화
					ft_memset(&clientAddr, '\0', addrSize);
					int clientSocket = accept(i, (struct sockaddr *)&clientAddr, &addrSize);
					if (clientSocket == -1)
						throw AcceptException();
					std::cout << "Open client socket: " << clientSocket
						<< ", port: " << servers[i].getPort() << std::endl;
					fcntl(clientSocket, F_SETFL, O_NONBLOCK);	// 해당 클라이언트 fd를 논블록으로 변경
					// 연결한 서버 소켓의 fd_set 비트 설정
					FD_SET(clientSocket, &rfds);
					FD_SET(clientSocket, &wfds);
					FD_SET(clientSocket, &efds);
					
					// 감시할 fd 범위 조정
					if (fdMax < clientSocket)
						fdMax = clientSocket;
					
					// clients map에 관리하는 클라이언트 정보 등록
					clients[clientSocket].setServerSocketFd(i);
					clients[clientSocket].setSocketFd(clientSocket);
					clients[clientSocket].setLastReqMs(ft_get_time());
				}
				else						// 클라이언트 소켓
				{
					int		len;				// 한 번의 read로 읽은 길이
					bool	isReadable = false;	// 더 읽을 수 있는지
					char	buf[bufferSize];	// read한 값 저장할 버퍼

					clients[i].setLastReqMs(ft_get_time());		// 마지막 연결 시간 업데이트
					// TODO: 한 번에 한 번씩 읽어야만 나중에 문제 안 생김 !수정 필요!
					while ((len = read(i, buf, bufferSize)) > 0)
					{
						isReadable = true;
						buf[len] = 0;
						clients[i].getRequest().setRawRequest(clients[i].getRequest().getRawRequest() + buf);
					}
					// 현재 데이터를 받고 있고 rawRequest가 파싱이 가능(CRLF 존재)할 때
					if (clients[i].getStatus() == REQUEST_RECEIVING && clients[i].getRequest().isParsable())
					{
						// 리퀘스트 파싱
						clients[i].getRequest().initRequest();
						clients[i].getRequest().parseRequest();
						std::string serverName = clients[i].getRequest().getHost();
						size_t	idx;
						if ((idx = serverName.find(':') ) != std::string::npos)
							serverName = serverName.substr(0, idx);
						// 클라이언트에 대해 리스폰스 생성
						clients[i].setResponse(ResponseHandler(clients[i].getRequest(),
							servers[clients[i].getServerSocketFd()]).makeResponse());
						clients[i].getRequest().initRequest();
						clients[i].setStatus(RESPONSE_READY);		// 현재 리스폰스 가능하다고 설정
					}
					// 읽을게 없다면
					if (isReadable == false)
					{
						clearConnectedSocket(i);		// 클라이언트와 연결 종료
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
				// timeout 시간이 지났을 때
				if (ft_get_time() - clients[i].getLastReqMs() > timeoutMs)
				{
					clearConnectedSocket(i);
					continue ;
				}
				// response를 줄 준비가 되었다면
				if (clients[i].getStatus() == RESPONSE_READY)
				{
					// 해당 내용을 클라이언트 소켓 fd에 write
					write(i, clients[i].getResponse().getMessage().c_str(), clients[i].getResponse().getMessage().size());
					if (clients[i].getResponse().getLastResponse() == 401)
						clearConnectedSocket(i);
					else
					{
						clients[i].getResponse().initResponse();
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
	updateFdMax();
	std::cout << "Connection close fd: " << fd << std::endl;
}

/*
 * 소켓 연결 종료시킬 때 fdMax값을 현재 열려 있는 fd 중 가장 큰 값으로 바꾸는 함수
 */
void	Socket::updateFdMax()
{
	int tmp = 2;

	for (std::map<int, Client>::iterator iter = clients.begin();
			iter != clients.end(); iter++)
		tmp = std::max(tmp, iter->first);
	for (std::map<int, Server>::iterator iter = servers.begin();
			iter != servers.end(); iter++)
		tmp = std::max(tmp, iter->first);
	fdMax = tmp;
}
