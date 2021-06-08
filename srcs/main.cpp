#include "webserv.h"
#include "Client.hpp"
#include "Socket.hpp"
#include "Location.hpp"
#include "Server.hpp"
#include "ServerConfig.hpp"
#include "Exception.hpp"

int		main(int argc, char *argv[])
{
    if (argc >= 3)
        throw ArgumentException();
    try
    {
        ServerConfig::getInstance()->saveConfig(argc, argv[1]);

        std::vector<Server> servers = ServerConfig::getInstance()->getServers();
        for (std::vector<Server>::iterator server_it = servers.begin();
                server_it != servers.end(); server_it++)
        {
            int listenPort = ft_atoi(server_it->getOption("listen").c_str());
            if (listenPort <= 0)
                throw WrongFileFormatException();
            std::cout << listenPort << std::endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        exit(1);
    }
    return (0);
}
