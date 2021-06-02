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
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}