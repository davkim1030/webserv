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
	if (checkLine(line)) // 공백 들어간 라인 제거
		return ;
	line = line.substr(0, line.find('#')).substr(0, line.find(';'));
	char *cLine = std::strtok((char *)line.c_str(), " \t");
	if (cLine == NULL)
		return ;
	std::pair<std::string, std::string> res;
	res.first = std::string(cLine);
	cLine = std::strtok(NULL, " \t");
	line.clear();
	while (cLine != 0)
	{
		line.append(cLine);
		cLine = std::strtok(NULL, " \t");
		if (cLine != 0)
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
	std::list<std::string> save;
	while (get_next_line(fd, &line))
	{
		std::string stdline(line);
		if (stdline.find('{') != std::string::npos)
		{
			save.push_back(stdline);
			parenFlag++;
		}
		else if (stdline.find('}') != std::string::npos)
		{
			save.push_back(stdline);
			parenFlag--;
			if (parenFlag == 0)
			{
				server.push_back(Server(save));
				save.clear();
			}
		}
		else if (parenFlag > 0)
			save.push_back(stdline);
		else
			configParse(stdline);
		free(line);
	}
	// parenFlag != 0 -> error

	// std::cout << "-----------config option print----------" << std::endl;
	// for (std::map<std::string, std::string>::iterator bit = option.begin(); bit != option.end(); bit++)
	// 	std::cout << "option : {" << bit->first << "} {" << bit->second << "}" << std::endl;
	
}