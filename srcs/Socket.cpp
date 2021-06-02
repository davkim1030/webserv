#include "Socket.hpp"

// 기본 생성자
Socket::Socket(int p) : serverSd(-1), clientSd(-1), port(p){}

// 소켓 생성 에외
const char *Socket::SocketException::what() const throw()
{
	return "Error: socket failed";
}
// 소켓 fd와 포트 바인딩 예외
const char *Socket::BindException::what() const throw()
{
	return "Error: bind failed";
}
// 소켓과 클라이언트 연결시 예외
const char *Socket::ListenException::what() const throw()
{
	return "Error: listen failed";
}

// 연결된 클라이언트의 데이터를 받을 시 에러
const char *Socket::AcceptException::what() const throw()
{
	return "Error: accept failed";
}

Socket::~Socket()
{
}

// 서버 초기 설정
void Socket::initServer(int socketCnt)
{
	ft_memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htons(INADDR_ANY);
	serverAddr.sin_port = htons(port);
	this->socketCnt = socketCnt;
	try
	{
		if (bind(serverSd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
			throw Socket::BindException();
		if (listen(serverSd, socketCnt) == -1)
			throw Socket::ListenException();	
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}	
}

void Socket::runServer()
{
	fd_set readfds, writefds, exceptfds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
	struct sockaddr clientAddr;
	size_t clientLen = sizeof(clientAddr);

    while(1)
    {
		if ((clientSd = accept(serverSd, (struct sockaddr *)&clientAddr, (socklen_t*)&clientLen)) >= 0)
		{
			struct timeval timeout;
			timeout.tv_sec = 5;
			timeout.tv_usec = 0;
			int selVal = select(socketCnt + 1, &readfds, &writefds, &exceptfds, &timeout);
			if (selVal > 0)
			{
				// do I/O operation
				operateIo(selVal, readfds, writefds, exceptfds);
			}
			else if (selVal == 0)
			{
				std::cout << "Time over" << std::endl;
			}
		}
    }
	close(clientSd);
}

/*
 * TODO: io 연산 이후 fd_set 비트 설정하기 전에 tmpFds에 복사하기
 * https://www.joinc.co.kr/w/Site/system_programing/File/select
 */
void Socket::operateIo(int maxSize, fd_set rfds, fd_set wfds, fd_set efds)
{
    int fd = 1;
	int opVal;
	fd_set tmpFds;
	char	buffer[100] = { 0, };
	size_t	bufferSize = 100;

    while (fd < maxSize)
    {
		FD_ZERO(&tmpFds);
        if (FD_ISSET(fd, &rfds))
        {
			opVal = read(fd, buffer, bufferSize);
			if (opVal >= 0)
			{
				// read file
				std::cout << "[read] " << buffer;
				ft_memset(buffer, '\0', 100);
			}
			else
			{
				// connection closed
				close(fd);
			}
			FD_SET(fd, &rfds);
			break ;
        }
        else if (FD_ISSET(fd, &wfds))
        {
			opVal = write(fd, buffer, bufferSize);
			if (opVal >= 0)
			{
				// read file
				std::cout << "[write] " << buffer;
				ft_memset(buffer, '\0', 100);
			}
			else
			{
				// connection closed
				close(fd);
			}
			FD_SET(fd, &rfds);
			break ;
        }
        else if (FD_ISSET(fd, &efds))
        {
			std::cout << "except fdset" << std::endl;
			break ;
        }
		fd++;
    }
}
