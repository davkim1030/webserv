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

int		main(int argc, char **argv)
{
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

	clnt_addr_size = sizeof(clnt_addr);
	while(1)
	{
		clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
		if (clnt_sock == -1)
			std::cout << "error : client accept error" << std::endl;

		// 클라이언트 인풋 read
		// 처리

		//result -> 클라이언트에 write
		char message[] = "Hello test world\n";
		write(clnt_sock, message, sizeof(message));

	}
	close(clnt_sock);
	close(serv_sock);


	return (0);
}
