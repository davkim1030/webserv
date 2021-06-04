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
		std::vector<Server>::iterator server_it;
		server_it = server.begin();
		std::cout << server_it->getOption("listen") << std::endl;
		server_it++;
		std::vector<Location> location = server_it->getLocationVector();
		std::cout << location.begin()->getPath() << std::endl;
		std::cout << "res {" << location.begin()->getOption("allow_metho") << "}" << std::endl;
		if (location.begin()->getOption("allow_metho").empty())
			std::cout << "NULL test" << std::endl;
		
		Location *loc = server_it->getLocation("/test/"); // 인자가 바뀜에 따라 NULL 또는 Location 주소 반환
		if (loc != NULL)
			std::cout << "location test : " << loc->getPath() << std::endl;
		else
			std::cout << "location can not found" << std::endl;
		
	    // 여기서부터 받아서 소켓 시작
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}
