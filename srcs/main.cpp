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
#include "Location.hpp"
#include "Server.hpp"
#include "ServerConfig.hpp"

# define CONFIG_PATH "./default.nginx"

int		main(int argc, char **argv)
{
	ServerConfig serverConfig;

	std::vector<int> ser;

	ser.push_back(2);

	char *line;
	if (argc > 2)
	{
		std::cout << "Error : argument error" << std::endl;
		exit(1);
	}
	int configFd;
	if (argc == 2)
		configFd = open(argv[1], O_RDONLY);
	else
		configFd = open(CONFIG_PATH, O_RDONLY);
	
	serverConfig.saveConfig(configFd);



	return (0);
}