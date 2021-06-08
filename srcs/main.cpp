#include "webserv.h"
#include "Client.hpp"
#include "Socket.hpp"
#include "Location.hpp"
#include "Server.hpp"
#include "ServerConfig.hpp"
#include "Exception.hpp"

int		main(int argc, char *argv[])
{
    Socket socket;

    try
    {
        if (argc >= 3)
            throw ArgumentException();
        // config 파일 파싱
        ServerConfig::getInstance()->saveConfig(argc, argv[1]);

        // for를 돌면서 각 서버 데이터에 대해서 소켓, 서버 시작
        std::vector<Server> servers = ServerConfig::getInstance()->getServers();
        for (std::vector<Server>::iterator server_it = servers.begin();
                server_it != servers.end(); server_it++)
        {
            // 서버 개수만큼 소켓 초기화
            socket.initServer(ServerConfig::getInstance()->getServers().size());
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        exit(1);
    }
    return (0);
}
