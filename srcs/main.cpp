#include "webserv.h"
#include "Exception.hpp"
# include "Response.hpp"
# include "Request.hpp"
# include "ResponseHandler.hpp"
# include "Server.hpp"
# include "ServerConfig.hpp"
# include "Location.hpp"


#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 80
int main(int argc, char *argv[])
{
    int server_fd, new_socket; long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    char *hello = strdup("Hello from server");

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    memset(address.sin_zero, '\0', sizeof address.sin_zero);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        char buffer[30000] = {0};
        valread = read( new_socket , buffer, 30000);

		// printf("%s",buffer);
        ServerConfig::getInstance()->saveConfig(argc, argv[1]);
        std::vector<Server> server = ServerConfig::getInstance()->getServers();
        /*
        std::vector<Server>::iterator server_it = server.begin();
        std::cout << server_it->getOption("listen") << std::endl;
        server_it++;
        std::vector<Location> location = server_it->getLocationVector();

        std::cout << location.begin()->getPath() << std::endl;

        std::cout << "res {" << location.begin()->getOption("allow_method") << "}" << std::endl;

        if (location.begin()->getOption("allow_method").empty())
            std::cout << "NULL test" << std::endl;

        Location *loc = server_it->getLocation("/test/"); // 인자가 바뀜에 따라 NULL 또는 Location 주소 반환

        if (loc != NULL)
            std::cout << "location test : " << loc->getPath() << std::endl;
        else
            std::cout << "location can not found" << std::endl;
        */

		Request Req(buffer);
		Req.parseRequest();
		ResponseHandler ReqHan(Req, server);
		Response Res = ReqHan.makeResponse();
		// std::cout << Res.getMessage();

		/*
			try
			{
				// 여기서부터 받아서 소켓 시작
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
		*/

	    write(new_socket , hello , strlen(hello));
        printf("------------------Hello message sent-------------------\n");
        close(new_socket);
    }
	close(server_fd);
}