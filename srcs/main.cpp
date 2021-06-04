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

		std::vector<Server> server = ServerConfig::getInstance()->getServers();
		std::vector<Server>::iterator it;
		it = server.begin();
		std::cout << it->getOption("listen") << std::endl;
		it++;
		std::vector<Location> location = it->getLocations();
		std::cout << location.begin()->getPath() << std::endl;
		std::cout << "res {" << location.begin()->getOption("allow_metho") << "}" << std::endl;
		if (location.begin()->getOption("allow_metho").empty())
			std::cout << "NULL test" << std::endl;


		// std::string test = server["listen"];
    // 여기서부터 받아서 소켓 시작
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}
