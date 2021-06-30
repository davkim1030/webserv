
#include "Socket.hpp"
#include "CgiResponse.hpp"
#include "ResponseHandler.hpp"
#include "CgiResource.hpp"
#include "ResponseMaker.hpp"
#include "ResourceHandler.hpp"


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
	usleep(1000);
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
		if (listen(iter->getFd(), ft_atoi(serverConfig.getOption("worker_processes").c_str())) == -1)
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
	fd_set cpyRfds;
	fd_set cpyWfds;
	fd_set cpyEfds;

	while (true)
	{
		usleep(5); // cpu 점유율 낮추기

		cpyRfds = this->rfds;
		cpyWfds = this->wfds;
		cpyEfds = this->efds;
		int fdNum = 0;
		if ((fdNum = select( this->fdMax + 1, &cpyRfds, &cpyWfds, &cpyEfds, &timeout)) == -1)
			throw SelectException();
		if (fdNum == 0)
			continue ;
		for (int i = 0; i < this->fdMax + 1; i++)
		{
			if (FD_ISSET(i, &cpyRfds)) // 읽기 요청
				doReadFdset(i);
			// else if (FD_ISSET(i, &cpyWfds)) // 쓰기 요청
			// 	doWriteFdset(i);
			// else if (FD_ISSET(i, &cpyEfds)) // 에러
			// 	doExceptFdset(i);
		}
	}
}

/*
 *
 */
void	Socket::doReadFdset(int currentFd)
{
	if (pool[currentFd]->getType() == SERVER) // 서버일 때
	{
		int					clientSocketFd;
		struct sockaddr_in	clientAddr;
		socklen_t			clientAddrSize = sizeof(clientAddr);

		// 서버로 들어온 클라이언트 연결 허용
		ft_memset(&clientAddr, '\0', clientAddrSize);
		clientSocketFd = accept(currentFd, (struct sockaddr *)&clientAddr, &clientAddrSize); // 클라이언트 요청 받기
		fcntl(clientSocketFd, F_SETFL, O_NONBLOCK);

		// 새로 생긴 클라이언트 등록 및 pool에 추가
		Client *tempClient = new Client(currentFd, clientSocketFd);
		tempClient->setLastReqMs(ft_get_time());
		pool[clientSocketFd] = dynamic_cast<IoObject *>(tempClient);


		std::cout << "Connection Client Fd open {" << clientSocketFd << "}" << std::endl;
		FD_SET(clientSocketFd, &this->rfds);
		FD_SET(clientSocketFd, &this->wfds);
		FD_SET(clientSocketFd, &this->efds);
		if (this->fdMax < clientSocketFd)
			this->fdMax = clientSocketFd;
	}
	else if (pool[currentFd]->getType() == CLIENT) // 클라이언트 일 때
	{
		// TODO: client read 자체가 client에서 이뤄지므로 client class로 넣을 수 있을듯 함
		Client *tempClient = dynamic_cast<Client *>(pool[currentFd]);

		std::cout << "client status : " << tempClient->getStatus() << std::endl;

		Status readingStatus = tempClient->readRequestMessage(pool, currentFd);
		tempClient->setStatus(readingStatus);
		if (readingStatus == DISCONNECTED)
			clearConnectedSocket(currentFd);
		if (readingStatus == RESPONSE_READY)
		{
			switch (prepareResponse(tempClient))
			case METHOD_NOT_ALLOWED :
			makeErrorResponse(METHOD_NOT_ALLOWED);

			case : 
			makeErrorResponse(401);

			default
			

		}

		// request 메시지가 다 들어온 경우 response 메시지 만들기
		// request가 header 인지 body 인지 확인 -> REQUEST_RECIEVING_HEAD && REQUEST_RECIEVING_BODY

		std::cout << "client status : " << tempClient->getStatus() << std::endl;
	}
	else if (pool[currentFd]->getType() == RESOURCE) // 리소스일 때 리소스는 일반 리소스 & cgi 리소스로 분류
	{

	}
}
//1. allowed 검사
//2. max-length검사
//3. autoindex, index 파일 검사
//4. resource 404 검사 (stat)
//-----------------------------------------
// open 검사
//-------------------------------------------
void Socket::prepareResponse(Client *client)
{

	std::cout << "prepare in " << std::endl;
	ftLog("test body", client->getWriteBuffer());
	ftLog("read buffer", client->getReadBuffer());

	
	// if (!location.getOption("allow_method").empty() && request.getMethod() != "GET" && request.getMethod() != "HEAD")
	// {
	// 	std::string allow = location.getOption("allow_method");

	// 	if (allow.find(request.getMethod()) == std::string::npos)
	// 	{
	// 		updateErrorStatus(clientFd, METHOD_NOT_ALLOWED);
	// 		return (false);
	// 	}
	// }
	// return (true);
	
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
	std::cout << "<<Connection close fd: " << fd << std::endl;
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

void	Socket::handleError(int clientFd, int selfFd, int statusCode)
{
	Client *clnt = dynamic_cast<Client *>(pool[clientFd]);
	clnt->setStatus(PROCESSING_ERROR);
	clnt->getResponse().setStatusCode(statusCode);
	FD_SET(clnt->getFd(), &wfds);
	clearConnectedSocket(selfFd);
}
