#include "webserv.h"
#include "Client.hpp"
#include "Socket.hpp"
#include "Location.hpp"
#include "Server.hpp"
#include "ServerConfig.hpp"
#include "Exception.hpp"

<<<<<<< HEAD

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 80
int main(int argc, char *argv[])
=======
int		main(int argc, char *argv[])
>>>>>>> main
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
            struct timeval timeout;
            timeout.tv_sec = 5;
            timeout.tv_usec = 0;
            socket.runServer(timeout, 100);
        }
<<<<<<< HEAD

        char buffer[30000] = {0};
        valread = read( new_socket , buffer, 30000);
        ServerConfig::getInstance()->saveConfig(argc, argv[1]);
        std::vector<Server> server = ServerConfig::getInstance()->getServers();
		Request request(buffer);
		request.parseRequest();
		ResponseHandler responseHandler(request, server.begin());
		Response Res = responseHandler.makeResponse();
	    write(new_socket , hello , strlen(hello));
        printf("------------------Hello message sent-------------------\n");
        close(new_socket);
=======
>>>>>>> main
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        exit(1);
    }
    return (0);
}
