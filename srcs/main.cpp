/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyukim <hyukim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/25 15:12:02 by hyukim            #+#    #+#             */
/*   Updated: 2021/05/25 15:14:28 by hyukim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.h"

#define PORT 8080

// FD_ZERO(fd_set* set);        //fdset을초기화
// FD_SET(int fd, fd_set* set);  //fd를 set에 등록
// FD_CLR(int fd, fd_set* set);  //fd를 set에서 삭제
// FD_ISSET(int fd, fd_set* set);//fd가 준비되었는지 확인


int		main(int argc, char **argv)
{

	if (argc != 2)
		std::cout << "Error : argument error" << std::endl;


	int serv_sock;
	int clnt_sock;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (serv_sock == -1)
		std::cout << "error : sock error" << std::endl;
	ft_memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT);

	if (bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == 1)
		std::cout << "error : bing error" << std::endl;
	
	if (listen(serv_sock, 5) == -1)
		std::cout << "error : listen error" << std::endl;

	fd_set reads;
	fd_set copyReads;

	fd_set writes;
	fd_set copyWrite;
	struct timeval timeout;

	int fd_max, str_len, fd_num;

	FD_ZERO(&reads); // fd_set 테이블 초기화
	FD_SET(serv_sock, &reads);

	fd_max = serv_sock;
	while(1)
	{
		copyReads = reads;
		copyWrite = writes;
		timeout.tv_sec = 5;
		timeout.tv_usec = 5000;

		if ((fd_num = select(fd_max + 1, &copyReads, &copyWrite, 0, &timeout)) == -1 )
		{
			std::cout << "select error" << std::endl;
			exit();
		}
		if (fd_num == 0)
			continue;

		for (int i = 0; i <= fd_max; i++)
		{
			if (FD_ISSET(fd, &copyReads))
			{
				if (i == serv_sock)
				{
					clnt_addr_size = sizeof(clnt_addr);
					clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
					if (clnt_sock == -1)
						std::cout << "error : client accept error" << std::endl;

					// 클라이언트 인풋 read
					// 처리

					//result -> 클라이언트에 write
					char message[] = "Hello test world\n";
					write(clnt_sock, message, sizeof(message));
				}
			}



		}

	}
	close(clnt_sock);
	close(serv_sock);


	return (0);
}
