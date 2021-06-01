#include "webserv.h"
#include "Location.hpp"

Location::Location() {}

Location::Location(Location const &lo)
{
	option = lo.option;
}

Location &Location::operator=(Location const &lo)
{
	if (this != &lo)
		option = lo.option;
	return *this;
}

Location::~Location() {}

void Location::configParse(std::string line)
{
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

Location::Location(std::list<std::string> &line)
{
	for (std::list<std::string>::iterator it = line.begin(); it != line.end(); it++)
		configParse(*it);

	// std::cout << "-----------location option print----------" << std::endl;
	// for (std::map<std::string, std::string>::iterator bit = option.begin(); bit != option.end(); bit++)
	// 	std::cout << "option : {" << bit->first << "} {" << bit->second << "}" << std::endl;
}