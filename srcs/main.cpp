#include "webserv.h"
#include "Client.hpp"
#include "Socket.hpp"
#include "Location.hpp"
#include "Server.hpp"
#include "ServerConfig.hpp"
#include "Exception.hpp"

void	sigintHandler(int signo)
{
	if (signo == SIGINT)
	{
		std::cout << "SIGINT Called" << std::endl;
		errno = 1;
	}
}

int		main(int argc, char *argv[])
{
	try
	{
		if (argc >= 3)
			throw ArgumentException();
		// config 파일 파싱 및 서버 설정 초기화
		Socket::getInstance()->initServer(argc, argv[1]);
		signal(SIGINT, sigintHandler);

		struct timeval timeout;
		timeout.tv_sec = 500000;
		timeout.tv_usec = 0;
		Socket::getInstance()->runServer(timeout);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		errno = 1;
		exit(1);
	}
	return (0);
}
