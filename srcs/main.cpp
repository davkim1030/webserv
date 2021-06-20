#include "webserv.h"
#include "Client.hpp"
#include "Socket.hpp"
#include "Location.hpp"
#include "Server.hpp"
#include "ServerConfig.hpp"
#include "Exception.hpp"

int		main(int argc, char *argv[])
{
	try
	{
		if (argc >= 3)
			throw ArgumentException();
		// config 파일 파싱 및 서버 설정 초기화
		Socket::getInstance()->initServer(argc, argv[1]);

		struct timeval timeout;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		Socket::getInstance()->runServer(timeout, 100);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		exit(1);
	}
	return (0);
}
