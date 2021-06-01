#include "webserv.h"
#include "Server.hpp"
#include "ServerConfig.hpp"

ServerConfig::ServerConfig(ServerConfig const &co) : option(co.option), server(co.server) {}

ServerConfig &ServerConfig::operator=(ServerConfig const &co)
{
	if (this != &co)
	{
		option = co.option;
		server = co.server;
	}
	return *this;
}

ServerConfig::~ServerConfig() {}

int ServerConfig::checkLine(std::string line)
{
	int i = 0;
	int res = 1;
	while (line[i])
	{
		if (ft_isalnum(line[i]))
			res = 0;
		i++;
	}
	return res;
}

void ServerConfig::configParse(std::string line)
{
	int index;
	if ((index = line.find('#')) != -1)
	{
		line = line.substr(0, index);
	}
	if (checkLine(line))
		return ;
	std::pair<std::string, std::string> res;
	line = line.substr(0, line.find(';'));

	char *test = std::strtok((char *)line.c_str(), " \t");
	if (test == NULL)
		return ;
	res.first = std::string(test);

	test = std::strtok(NULL, " \t");
	line.clear();
	while (test != 0)
	{
		line.append(test);
		test = std::strtok(NULL, " \t");
		if (test != 0)
			line.append(" ");
	}
	res.second = line;
	if (res.second.empty() || res.first.empty())
		return ;
	option[res.first] = res.second;
}

void ServerConfig::saveConfig(int fd)
{
	int parenFlag = 0;
	char *line;
	std::string stdline;
	while (get_next_line(fd, &line))
	{
		std::string test(line);
		if (test.find('{') != -1)
		{
			stdline.append(test + '\n');
			parenFlag++;
		}
		else if (test.find('}') != -1)
		{
			stdline.append(test + '\n');
			parenFlag--;
			if (parenFlag == 0)
			{
				server.push_back(Server(stdline));
				stdline.clear();
			}
		}
		else if (parenFlag > 0)
		{
			stdline.append(test + '\n');
		}
		else
		{
			configParse(test);
		}
		free(line);
	}

	std::map<std::string, std::string>::iterator it;

	for(it = option.begin(); it != option.end(); it++)
		std::cout << "{" << it->first << "} {" << it->second << "}" << std::endl;
}