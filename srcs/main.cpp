#include "webserv.h"
#include "Location.hpp"
#include "Server.hpp"
#include "ServerConfig.hpp"

int		main(int argc, char **argv)
{
	if (argc > 2)
	{
		std::cout << "Error : argument error" << std::endl;
		exit(1);
	}

	try
	{
		ServerConfig::getInstance()->saveConfig(argc, argv[1]);
		ServerConfig::getInstance()->printItem();
    // 여기서부터 받아서 소켓 시작
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}
