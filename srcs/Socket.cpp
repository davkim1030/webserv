#include "Socket.hpp"
#include "CgiResponse.hpp"
#include "NormalResponse.hpp"
#include "CgiResource.hpp"
#include "ResponseHandler.hpp"

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
	// unsigned long	timeoutMs = timeout.tv_sec * 1000 + timeout.tv_usec / 1000;

	// 메인 루프
	while (1)
	{
		// fds들의 데이터 유실 방지를 위해 복사
		cpyRfds = rfds;	// 0 0 0 1 0
		cpyWfds = wfds;	// 0 0 0 0 1
		cpyEfds = efds;	// 0 0 0 0 1 

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
				// 서버 읽기(클라이언트 연결 생성)
				if (pool[i]->getType() == SERVER)
				{
					int clientSocket;
					struct sockaddr_in	clientAddr;
					socklen_t	addrSize = sizeof(clientAddr);

					ft_memset(&clientAddr, '\0', addrSize);
					clientSocket = accept(i, (struct sockaddr *)&clientAddr, &addrSize);
					if (clientSocket == -1)
						throw AcceptException();
					fcntl(clientSocket, F_SETFL, O_NONBLOCK);	// 해당 클라이언트 fd를 논블록으로 변경
					// 연결한 서버 소켓의 fd_set 비트 설정
					FD_SET(clientSocket, &rfds);
					FD_SET(clientSocket, &efds);
					
					// 감시할 fd 범위 조정
					if (fdMax < clientSocket)
						fdMax = clientSocket;
					
					// clients map에 관리하는 클라이언트 정보 등록
					Client *tmpClient = new Client(i, clientSocket);
					tmpClient->setLastReqMs(ft_get_time());
					pool[clientSocket] = dynamic_cast<IoObject *>(tmpClient);
				}
				// 클라이언트 리퀘스트 받는 부분
				else if (pool[i]->getType() == CLIENT)
				{
					int readLen;
					char buf[IO_BUFFER_SIZE + 1];
					Client *tmpClient = dynamic_cast<Client *>(pool[i]);

					ft_memset(buf, '\0', IO_BUFFER_SIZE + 1);
					if ((readLen = read(i, buf, IO_BUFFER_SIZE)) > 0)
					{
						buf[readLen] = '\0';
						tmpClient->setBuffer(tmpClient->getBuffer() + buf);
					}
					else	// error case
					{
						clearConnectedSocket(i);
					}

					// 헤더 파싱 가능한지 확인
					if (tmpClient->getStatus() == REQUEST_RECEIVING_HEADER && tmpClient->headerParsable())
					{
						tmpClient->setBuffer(tmpClient->getRequest().parseFirstLine(tmpClient->getBuffer()));	// 첫 줄 파싱
						std::map<std::string, std::string> header = Request::parseHeader(tmpClient->getBuffer());
						tmpClient->getRequest().setHeader(header);
						tmpClient->getRequest().setLocation(findLocation(*dynamic_cast<Server *>(pool[tmpClient->getServerSocketFd()]),
								tmpClient->getRequest().getUri()));
						
						tmpClient->setBuffer(tmpClient->getBuffer().substr(tmpClient->getBuffer().find("\r\n\r\n") + 4));
						tmpClient->setStatus(REQUEST_RECEIVING_BODY);
						// cgi 실행 해주기
						if (isCgi(tmpClient->getRequest().getUri(), tmpClient->getRequest().getLocation()))
						{
							CgiResponse cgiResponse(tmpClient->getRequest(), tmpClient->getServer(), tmpClient->getRequest().getLocation());
							cgiResponse.makeVariable(i);
							cgiResponse.cgiResponse(i);
						}
						else
						{
							ResponseHandler ResHan(tmpClient->getRequest(), tmpClient->getRequest().getLocation(), i);
							int stat;
							if ((stat = ResHan.checkAllowMethod()) != CHECK_SUCCES)
								updateErrorStatus(i, stat);
							//Request가 Resource를 필요로하는 타입인지 확인
							if (tmpClient->getRequest().getMethod == "GET")
							{
								if ((stat = ResHan.checkGetMethodIndex()) != CHECK_SUCCES)
									updateErrorStatus(i, stat);
								else if ((stat = ResHan.tryToRead()) != CHECK_SUCCES)
									updateErrorStatus(i, stat);
								else
									ResHan.setReadFlag();
							}
							else if (tmpClient->getRequest().getMethod == "POST" || tmpClient->getRequest().getMethod == "PUT")
							{
								if ((stat = ResHan.tryToWrite()) != CHECK_SUCCES)
									updateErrorStatus(i, stat);
								else
									ResHan.setWriteFlag();
							}
						}
					}

					// chunked/content-length 처리해서 바디 파싱 가능한지 확인
					if (tmpClient->getStatus() == REQUEST_RECEIVING_BODY)
					{
						// Chunked일 경우  -> 모두 붙여서 전달
						if (tmpClient->getRequest().getHeader().count("Transfer-Encoding") == 1 &&
								tmpClient->getRequest().getHeader()["Transfer-Encoding"] == "chunked")
						{
							if (tmpClient->getBuffer().find("0\r\n\r\n") != std::string::npos)
							{
								tmpClient->getRequest().setRawBody(Request::parseChunkedBody(tmpClient->getBuffer()));
							}
							tmpClient->setStatus(RESPONSE_READY);
						}
						// Content-Length인 경우 -> 들어온 애들을 모두 붙인 후에 substr(Content-Length) 후 전달
						else if (tmpClient->getRequest().getHeader().count("Content-Length") == 1)
						{
							int contentLength = ft_atoi(tmpClient->getRequest().getHeader()["Content-Length"].c_str());
							if (tmpClient->getBuffer().length() >= (size_t)contentLength)
								tmpClient->getRequest().setRawBody(tmpClient->getBuffer().substr(0, contentLength));
							tmpClient->setStatus(RESPONSE_READY);
						}
						// Chunked, Content-Length 모두 없는 경우
						else if (tmpClient->getRequest().getHeader().count("Content-Length") == 0)
						{
							tmpClient->getRequest().getHeader()["Content-Length"] = "0";
							tmpClient->getRequest().setRawBody("");
							tmpClient->setStatus(RESPONSE_READY);
						}
						// 리소스 타입판별 (normalResponse 반으로 쪼개기)'
						// response ready function call

					}

					ftLog("REQUEST", tmpClient->getBuffer());
					// !!! 여기서 ResponseHandler를 호출해야함, 클라이언트의 fd는 i임
					// PUT, POST가 아닐 경우 여기서 아래 함수로 진입
					// NormalResponse normalRes(tmpClient->getRequest(), tmpClient->getLocation(), *dynamic_cast<Server *>(pool[tmpClient->getServerSocketFd()]);
					// 리스폰스핸들러에서 get_next_line, response 만드는 부분, write 삭제

					// 정적 리소스 파일이 없는 경우 ->response_ready인데, response를 만들어서 줘야되네여
					// 둘 다 아닌 경우
				}
				// 리소스 읽기 동작
				else if (pool[i]->getType() == RESOURCE)
				{
					Resource *tmpRsrc = dynamic_cast<Resource *>(pool[i]);

					char	buf[IO_BUFFER_SIZE + 1];
					ft_memset(buf, '\0', IO_BUFFER_SIZE + 1);
					size_t	readLen = read(tmpRsrc->getFd(), buf, IO_BUFFER_SIZE);
					buf[readLen] = '\0';

					if (readLen == -1)
					{
						// 500 서버 에러 띄워주기
					}
					else if (readLen == 0)
					{
						pool[tmpRsrc->getClientFd()]->setBuffer(tmpRsrc->getBuffer());
						FD_CLR(i, &rfds);
						FD_SET(tmpRsrc->getClientFd(), &wfds);
						clearConnectedSocket(i);
					}
					else
					{
						tmpRsrc->setBuffer(tmpRsrc->getBuffer() + buf);
					}
				}
				// CGI 리소스 읽기
				else if (pool[i]->getType() == CGI_RESOURCE)
				{
					CgiResource *tmpCgi = dynamic_cast<CgiResource *>(pool[i]);
					int status;

					if (waitpid(tmpCgi->getPid(), &status, WNOHANG) == 0)
						continue ;
					if (tmpCgi->isLseek() == false)
					{
						lseek(tmpCgi->getFd(), 0, SEEK_SET);
						tmpCgi->setLseek();
						struct stat sb;
						char *num = ft_itoa(tmpCgi->getClientFd());
						if (stat(std::string("./cgi_files/cgi_result" + std::string(num)).c_str() ,&sb) == -1)
							std::cerr << "stat err!" << std::endl;
						free(num);
						tmpCgi->setFileSize(sb.st_size);
					}

					char buf[IO_BUFFER_SIZE + 1];
					ft_memset(buf, '\0', IO_BUFFER_SIZE + 1);
					int readLen = read(tmpCgi->getFd(), buf, IO_BUFFER_SIZE);
					buf[readLen] = '\0';
					if (readLen == -1)
					{
						// 500 서버 에러 띄워주기
					}
					else if (readLen == 0)
					{
						pool[tmpCgi->getClientFd()]->setBuffer(tmpCgi->getBuffer());
						FD_CLR(i, &rfds);
						FD_SET(tmpCgi->getClientFd(), &wfds);
						clearConnectedSocket(i);
					}
					else
					{
						tmpCgi->setBuffer(tmpCgi->getBuffer() + buf);
					}
				}
			}
			else if (FD_ISSET(i, &cpyWfds))	// write 요청이 온 경우
			{
				// 클라이언트 쓰기(리스폰스 주는 곳)
				if (pool[i]->getType() == CLIENT)
				{
					Client *tmpClnt = dynamic_cast<Client *>(pool[i]);
					if (tmpClnt->getStatus() == PROCESSING_ERROR)
					{
						ResponseMaker resMaker(tmpClnt->getRequest(), tmpClnt->getServer(), tmpClnt->getRequest().getLocation());
						Response r = resMaker.makeErrorResponse(tmpClnt->getResponse().getStatusCode(), tmpClnt->getRequest().getHttpVersion());
						write(i, r.getMessage().c_str(), r.getMessage().length());
					}
					else if (isCgi(tmpClnt->getRequest().getUri(), tmpClnt->getRequest().getLocation()))
					{
						std::string msg = CgiResponse::cgiResultPasring(tmpClnt->getBuffer()).getMessage();
						write(i, msg.c_str(), msg.length());
					}
					else
					{
						//리스펀스만드는곳!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					}
					clearConnectedSocket(i);
				}
				// 리소스 쓰기 (PUT, POST 등)
				else if (pool[i]->getType() == RESOURCE)
				{
					Resource *tmpRsrc = dynamic_cast<Resource *>(pool[i]);
					
					int writeLen = write(tmpRsrc->getFd(), tmpRsrc->getBuffer().c_str() + tmpRsrc->getPos(), tmpRsrc->getBuffer().length() - tmpRsrc->getPos());
					if (writeLen == -1)
						clearConnectedSocket(i);
					else
					{
						if (tmpRsrc->getPos() + writeLen < tmpRsrc->getBuffer().length())
							tmpRsrc->setPos(tmpRsrc->getPos() + writeLen);
						else
							clearConnectedSocket(i);
					}
				}
				// CGI에 Request body를 파이프로 넘겨주는 곳
				else if (pool[i]->getType() == CGI)
				{
					CgiWriter *tmpCgi = dynamic_cast<CgiWriter *>(pool[i]);
					std::string body = pool[tmpCgi->getClientFd()]->getBuffer();

					int writeLen = write(tmpCgi->getFd(), body.c_str() + tmpCgi->getPos(), body.length() - tmpCgi->getPos());
					if (writeLen == -1)
						clearConnectedSocket(i);
					else
					{
						if (tmpCgi->getPos() + writeLen < body.length())
							tmpCgi->setPos(tmpCgi->getPos() + writeLen);
						else
							clearConnectedSocket(i);
					}

					/*
						write를 해주는 경우 == body를 입력할 수 있는 경우
						body가 준비되어있어야한다 or 들어오는대로 body를 넣어주는데 
					*/

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

/*
 * request의 uri와 매칭되는 로케이션 반환
 */
Location Socket::findLocation(Server server, std::string uri)
{
	std::vector<Location> ser = server.getLocationVector();
	Location res;
	for (std::vector<Location>::iterator it = ser.begin(); it != ser.end(); it++)
	{
		std::string path = it->getPath();
		std::string tempPath = path + '/';
		if (uri.compare(0, path.length(), path) == 0 || uri.compare(0, tempPath.length(), tempPath) == 0)
			res = *it;
	}
	return res;
}

// cgi 실행 여부 판단
bool Socket::isCgi(std::string rawUri, Location location)
{
	std::string uri = rawUri.substr(location.getPath().length());
	std::vector<std::string> ext = location.getCgiExtensionVector();

	for (std::vector<std::string>::iterator it = ext.begin(); it != ext.end(); it++)
	{
		size_t index = uri.find(*it);
		if (index != std::string::npos && uri.compare(index, it->length(), *it) == 0)
		{
			int queryIndex;
			if ((queryIndex = uri.find('?')) != -1)
				uri = uri.substr(0, queryIndex);
			uri = uri.substr(index);
			size_t pathIndex = uri.find('/');
			if (pathIndex != std::string::npos)
				uri = uri.substr(0, pathIndex);
			if(uri.compare(0, it->length() + 1, *it) != 0)
				return false;
			return true;
		}
	}
	return (false);
}

void Socket::updateFds(int fd, FdType fdType)
{
	if (fdType == FD_READ)
		FD_SET(fd, &rfds);
	else if (fdType == FD_WRITE)
		FD_SET(fd, &wfds);
	else
		FD_SET(fd, &efds);
	updateFdMax();
}
