#include "Socket.hpp"

// 싱글톤 사용을 위한 변수 선언
Socket *Socket::instance;

/*
 * 싱글톤 패턴을 위해 Socket 객체를 반환하는 함수
 */
Socket *Socket::getInstance()
{
	if (instance == NULL)
		instance = new Socket();
	return (instance);
}

Socket::Socket(Socket const &so)
: rfds(so.rfds), wfds(so.wfds), efds(so.efds), fdMax(so.fdMax),
	serverConfig(so.serverConfig), pool(so.pool)
{
}

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
Socket::Socket() : fdMax(-1), serverConfig(ServerConfig()), pool(1024, NULL)
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
	for (std::vector<IoObject *>::iterator iter = pool.begin();
			iter != pool.end(); iter++)
	{
		if (*iter != NULL)
		{
			close((*iter)->getFd());
			delete *iter;
		}
	}
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
 * nginx config를 파싱한 데이터를 우선 파싱한다.
 * 그 데이터를 기반으로 각 서버의 데이터를 가져와서 servers 변수에 등록
 * @param int argc: 커맨드라인의 인자 개수(argc)
 * @param int argv: 커맨드라인에서 받은 파일 이름(argv[1])
 */
void Socket::initServer(int argc, char *argv)
{
	std::map<std::string, int> tmp;

	serverConfig.saveConfig(argc, argv);		// 서버 컨피그 파싱
	// 모든 서버 컨피그 파싱 값을 기반으로 서버 소켓 열고 데이터 생성
	std::vector<Server>::iterator iter;
	for (iter = serverConfig.getServers().begin();
		iter != serverConfig.getServers().end(); iter++)
	{
		char *strTmp = ft_itoa(iter->getPort());
		std::string key = iter->getIp() + ":" + strTmp;
		free(strTmp);
		std::cout << "IP: " << iter->getIp() << "$" << std::endl;
		std::cout << "ServerName: " << iter->getServerName() << "$" << std::endl;
		std::cout << "port: " << iter->getPort() << "$" << std::endl;

		// 이전에 존재하던 key이면 내용 읽지 않고 패스
		if (tmp.find(key) != tmp.end())
			continue ;

		struct sockaddr_in	serverAddr;							// 서버 소켓의 ip주소
		iter->setFd(socket(PF_INET, SOCK_STREAM, 0));			// 소켓 fd 생성
		int reuseAddr = 1;
		// 이전 포트 사용하게 설정
		setsockopt(iter->getFd(), SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(int));
		ft_memset(&serverAddr, '\0', sizeof(serverAddr));		// serverAddr 초기화
		serverAddr.sin_family = AF_INET;						// IPv4 설정
		serverAddr.sin_addr.s_addr = inet_addr(iter->getIp().c_str());	// ip 주소 설정
		serverAddr.sin_port = htons(iter->getPort());			// 포트 설정

		// 소켓, fd 바인드
		if (bind(iter->getFd(), (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
			throw BindException();
		// 데이터를 받을 서버 소켓 열기
		if (listen(iter->getFd(), serverConfig.getServers().size()) == -1)
			throw ListenException();
		
		std::cout << iter->getIp() << ":" << iter->getPort() << std::endl;
		
		// 생성한 서버 소켓 fd 값을 fd_set 변수들에 대해 설정
		FD_SET(iter->getFd(), &rfds);
		FD_SET(iter->getFd(), &wfds);
		FD_SET(iter->getFd(), &efds);

		pool[iter->getFd()] = new Server(*iter);
		tmp[key] = iter->getFd();			// 중복 방지를 위해 tmp map에 추가

		// select()로 감시할 fd의 최대값 업데이트
		if (fdMax < iter->getFd())
			fdMax = iter->getFd();
	}
}

/*
 * 실제 서버를 동작시키는 메인 로직
 * 꼭 initServer를 호출하여 서버 정보들 등록 후 동작 시켜야 함.
 * @param struct timeval timeout: 한 클라이언트에서 최대 대기할 시간(ms)
 */
void Socket::runServer(struct timeval timeout)
{
	fd_set	cpyRfds;
	fd_set	cpyWfds;
	fd_set	cpyEfds;
	int		fdNum;		// read/write/except 신호가 set된 fd의 개수
	unsigned long	timeoutMs = timeout.tv_sec * 1000 + timeout.tv_usec / 1000;

	// 메인 루프
	while (1)
	{
		// fds들의 데이터 유실 방지를 위해 복사
		cpyRfds = rfds;
		cpyWfds = wfds;
		cpyEfds = efds;

		// fd_set 변수의 0 ~ fdMax + 1까지 비트를 감시하여 읽기, 쓰기, 에러 요구가 일어났는지 확인
		if ((fdNum = select(fdMax + 1, &cpyRfds, &cpyWfds, &cpyEfds, &timeout)) == -1)
			throw SelectException();
		if (fdNum == 0) // 처리할 요구가 없으면 다시 위로
			continue ;
		
		printFdsStatus(rfds, wfds, efds, fdMax);
		// fd를 0부터 fdMax까지 반복하며 set된 플래그 값이 있는지 확인하여 처리
		for (int i = 0; i < fdMax + 1; i++)
		{
			if (pool[i] == NULL)
				continue ;
			if (FD_ISSET(i, &cpyRfds))	// read 요청이 온 경우
			{
				if (pool[i]->getType() == SERVER)	// 서버 소켓의 경우
				{
					struct sockaddr_in	clientAddr;
					socklen_t	addrSize = sizeof(clientAddr);

					// 주소 변수 데이터 초기화
					ft_memset(&clientAddr, '\0', addrSize);
					int clientSocket = accept(i, (struct sockaddr *)&clientAddr, &addrSize);
					if (clientSocket == -1)
						throw AcceptException();
					std::cout << "Open client socket: " << clientSocket
						<< ", port: " << dynamic_cast<Server *>(pool[i])->getPort() << std::endl;
					fcntl(clientSocket, F_SETFL, O_NONBLOCK);	// 해당 클라이언트 fd를 논블록으로 변경
					// 연결한 서버 소켓의 fd_set 비트 설정
					FD_SET(clientSocket, &rfds);
					FD_SET(clientSocket, &wfds);
					FD_SET(clientSocket, &efds);
					
					// 감시할 fd 범위 조정
					if (fdMax < clientSocket)
						fdMax = clientSocket;
					
					// pool에 관리하는 클라이언트 정보 등록
					Client *tmpClient = new Client(i, clientSocket);
					tmpClient->setLastReqMs(ft_get_time());
					pool[clientSocket] = tmpClient;
				}
				else if (pool[i]->getType() == CLIENT)	// 클라이언트 소켓
				{
					Client	*tmpClient;
					int		len;				// 한 번의 read로 읽은 길이
					bool	isReadable = false;	// 더 읽을 수 있는지
					char	buf[IO_BUFFER_SIZE + 1];	// read한 값 저장할 버퍼

					tmpClient = dynamic_cast<Client *>(pool[i]);
					tmpClient->setLastReqMs(ft_get_time());		// 마지막 연결 시간 업데이트
					if ((len = read(i, buf, IO_BUFFER_SIZE)) > 0)
					{
						isReadable = true;
						buf[len] = 0;
						tmpClient->setBuffer(tmpClient->getBuffer() + buf);
					}
					// 현재 데이터를 받고 있고 request의 header를 파싱이 가능(CRLF 존재)할 때
					// TODO: 헤더 파싱이 가능하면 -> 헤더만 파싱
					if (tmpClient->getStatus() == REQUEST_RECEIVING_HEADER && tmpClient->headerParsable())
					{
						tmpClient->getRequest().initRequest();
						std::cout << tmpClient->getBuffer() << std::endl;
						tmpClient->getRequest().parseFirstLine(tmpClient->getBuffer());
						tmpClient->setBuffer(tmpClient->getBuffer().substr(tmpClient->getBuffer().find("\r\n", 0) + 2));
						std::map<std::string, std::string> tmpHeader
							= Request::parseHeader(tmpClient->getBuffer());
						tmpClient->getRequest().setHeader(tmpHeader);
						tmpClient->setBuffer(tmpClient->getBuffer().substr(tmpClient->getBuffer().find("\r\n\r\n") + 4));
						tmpClient->setStatus(REQUEST_RECEIVING_BODY);
					}
					// TODO: 바디도 파싱 가능하면 그 때서야 바디 파싱
					Response tmpRes = ResponseHandler(tmpClient->getRequest(),
							*(dynamic_cast<Server *>(pool[tmpClient->getServerSocketFd()]))).makeResponse();
					if (tmpClient->getStatus() == REQUEST_RECEIVING_BODY && (tmpClient->bodyParsable() || tmpRes.getStatusCode() != 0))
					{
						// 클라이언트에 대해 리스폰스 생성
						tmpClient->setResponse(tmpRes);
						tmpClient->setBuffer(tmpClient->getResponse().getMessage());
						tmpClient->getRequest().initRequest();
						tmpClient->setStatus(RESPONSE_READY);		// 현재 리스폰스 가능하다고 설정
					}
					// 읽을게 없다면
					if (isReadable == false)
					{
						clearConnectedSocket(i);		// 클라이언트와 연결 종료
						if (len == 0)
							std::cout << "Disconnected " << i << " in Server" << std::endl;
						else
							std::cout << "Error occured client " << i << " in Server" << std::endl;
					}
				}
				else if (pool[i]->getType() == RESOURCE)		// 리소스 읽기 경우
				{
					Resource	*tmpResource;
					int		len;				// 한 번의 read로 읽은 길이
					bool	isReadable = false;
					char	buf[IO_BUFFER_SIZE + 1];	// read한 값 저장할 버퍼

					tmpResource = dynamic_cast<Resource *>(pool[i]);
					if ((len = read(i, buf, IO_BUFFER_SIZE)) > 0)
					{
						buf[len] = 0;
						isReadable = true;
						tmpResource->setBuffer(tmpResource->getBuffer() + buf);
					}
					else if (len == 0)
					{
						// 읽은게 없다면
						buf[len] = 0;
						tmpResource->setBuffer(tmpResource->getBuffer() + buf);
						tmpResource->setIoStatus(DONE);
					}
					else
						tmpResource->setIoStatus(ERROR);
					if (isReadable == false)
					{
						clearConnectedSocket(i);
						if (len < 0)
							std::cout << "Error occured while reading static file " << i << " in Server" << std::endl;
					}
				}
			}
			else if (FD_ISSET(i, &cpyWfds))	// write 요청이 온 경우
			{
				if (pool[i]->getType() == CLIENT)
				{
					Client *tmpClient = dynamic_cast<Client *>(pool[i]);
					// timeout 시간이 지났을 때
					if (ft_get_time() - tmpClient->getLastReqMs() > timeoutMs)
					{
						clearConnectedSocket(i);
						continue ;
					}
					// response를 줄 준비가 되었다면
					if (tmpClient->getStatus() == RESPONSE_READY)
					{
						if (tmpClient->getResponse().getLastResponse() == 401)
							clearConnectedSocket(i);
						// 해당 내용을 클라이언트 소켓 fd에 write
						int writeLen = 0;
						if (tmpClient->getBuffer().empty() == false)
							tmpClient->setBuffer(tmpClient->getResponse().getMessage());
						writeLen = write(i, tmpClient->getBuffer().c_str(), tmpClient->getBuffer().size());
						if (writeLen != (int)tmpClient->getBuffer().size())
						{
							tmpClient->setBuffer(tmpClient->getBuffer().substr(writeLen));
						}
						else
						{
							tmpClient->getResponse().initResponse();
							tmpClient->setStatus(REQUEST_RECEIVING_HEADER);
							clearConnectedSocket(i);
							FD_CLR(i, &wfds);
						}
					}
				}
				else if (pool[i]->getType() == RESOURCE)
				{
					// reource 쓰기 경우
					Resource *tmpResource = dynamic_cast<Resource *>(pool[i]);
					// response를 줄 준비가 되었다면
					if (tmpResource->getStatus() == RESPONSE_READY)
					{
						// 해당 내용을 리소스 fd에 write
						int writeLen = 0;
						writeLen = write(i, tmpResource->getBuffer().c_str(), tmpResource->getBuffer().size());
						if (writeLen != (int)tmpResource->getBuffer().size())
						{
							tmpResource->setBuffer(tmpResource->getBuffer().substr(writeLen));
						}
						else
						{
							clearConnectedSocket(i);
							FD_CLR(i, &wfds);
						}
					}
				}
			}
			else if (FD_ISSET(i, &cpyEfds))	// except 요청이 온 경우
			{
				clearConnectedSocket(i);
			}
		}
		usleep(5);	// cpu 100% 점유 방지
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

	if (pool[fd] != NULL)
	{
		close(fd);
		delete pool[fd];
		pool[fd] = NULL;
	}
	updateFdMax();
	std::cout << "Connection close fd: " << fd << std::endl;
}

/*
 * 소켓 연결 종료시킬 때 fdMax값을 현재 열려 있는 fd 중 가장 큰 값으로 바꾸는 함수
 */
void	Socket::updateFdMax()
{
	int tmp = 2;

	for (std::vector<IoObject *>::iterator iter = pool.begin();
			iter != pool.end(); iter++)
	{
		if (*iter != NULL)
			tmp = (*iter)->getFd();
	}
	fdMax = tmp;
}

std::vector<IoObject *> &Socket::getPool()
{
	return (pool);
}
