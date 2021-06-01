#include "webserv.h"
#include "Location.hpp"
#include "Server.hpp"

Server::Server() {}

Server::Server(Server const &serv) : option(serv.option), location(serv.location)
{}

Server &Server::operator=(Server const &serv)
{
    if (this != &serv)
    {
        option = serv.option;
        location = serv.location;
    }
    return *this;
}

Server::~Server() {}

int Server::checkLine(std::string line)
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

void Server::configParse(std::string line)
{
	if (checkLine(line))
		return ;
	line = line.substr(0, line.find('#')).substr(0, line.find(';'));
	char *cLine = std::strtok((char *)line.c_str(), " \t");
	if (cLine == NULL)
		return ;
	std::pair<std::string, std::string> res;
	res.first = std::string(cLine);
	if (res.first.compare(0, 7, "server\0") == 0)
		return ;
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



Server::Server(std::list<std::string> &strlst)
{
	int parenFlag = 0;
	std::list<std::string> save;

	for (std::list<std::string>::iterator it = strlst.begin(); it != strlst.end(); it++)
	{
		if (it->find('{') != -1 && it->find("server") == -1)
		{
			// save.push_back(*it);
			parenFlag++;
		}
		else if (it->find('}') != -1)
		{
			// save.push_back(*it);
			parenFlag--;
			if (parenFlag == 0 && it != --strlst.end())
			{
				location.push_back(Location(save));
				save.clear();
			}
		}
		else if (parenFlag > 0)
		{
			save.push_back(*it);
		}
		else
		{
			configParse(*it);
		}
	}
	// parenFlag != -1 -> error

	// std::cout << "=---------------server option print ------------=" << std::endl;
	// for (std::map<std::string, std::string>::iterator lit = option.begin(); lit != option.end(); lit++)
	// {
	// 	std::cout << "save : {" << lit->first << "} {" << lit->second << "}" << std::endl;
	// }
}