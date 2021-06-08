#include "webserv.h"
#include "Location.hpp"
#include "Server.hpp"
#include "Exception.hpp"

Server::Server() {}

Server::Server(Server const &serv) : option(serv.option), location(serv.location),
		ip("127.0.0.1"), port(0), serverName("_"), socketFd(-1)
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

/*
	줄이 공백만 존재하는지 체크
	@param -> std::string line : config 파일 내부의 한 줄
*/
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

/*
	line을 받아서 key : value 형태로 저장
	줄에 server 글자가 들어있을 경우엔 저장하지 않고 pass
	@param -> std::string line : location 블럭 외에 server가 가질 옵션을 가진 문자열
*/
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

/*
	private 멤버 변수 location 반환 getter
*/
std::vector<Location> Server::getLocationVector()
{
	return location;
}

/*
	Server가 가진 Location Vector에서 name과 일치하는 path를 가진 Location 반환
	@param -> std::list<std::string> path -> 찾으려는 path
*/
Location *Server::getLocation(std::string const& path)
{
	std::vector<Location>::iterator it;
	for (it = location.begin(); it != location.end(); it++)
	{
		if (it->getPath() == path)
			return &*it;
	}
	return NULL;
}

/*
	ServerConfig에서 만든 server블럭을 받아 파싱 및 생성해주는 생성자
	location으로 시작되는 블럭이 있으면 Location 클래스로 저장해 location변수에 저장
	그 외에는 server의 option으로 저장
	파일에 중괄호가 올바르게 있지 않을 경우 throw WrongFileFormat
	@param -> std::list<std::string> strlst -> 파싱에 쓰일 server블럭을 저장한 list 개행 단위로 잘라 한 노드로 저장되어있음
*/
Server::Server(std::list<std::string> &strlst)
{
	int parenFlag = 0;
	std::list<std::string> save;

	for (std::list<std::string>::iterator it = strlst.begin(); it != strlst.end(); it++)
	{
		if (it->find('{') != std::string::npos && it->find("server") == std::string::npos)
		{
			save.push_back(*it);
			parenFlag++;
		}
		else if (it->find('}') != std::string::npos)
		{
			save.push_back(*it);
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
	if (option.find("listen") != option.end())
		port = static_cast<unsigned int>(ft_atoi(option["listen"].c_str()));
	else
		parenFlag = -1;
	splitSpaces(option["listen"]);
	// parenFlag != -1 -> error
	if (parenFlag != -1)
		throw WrongFileFormatException();
}

/*
	location 멤버변수 option[key] 값을 반환
	@param -> key -> option에 저장된 key와 매칭되는 value를 반환
*/
std::string Server::getOption(std::string const &key)
{
	return option[key];
}

const std::string &Server::getIp() const
{
	return (ip);
}

unsigned int Server::getPort() const
{
	return (port);
}

const std::string &Server::getServerName() const
{
	return (serverName);
}

int Server::getSocketFd() const
{
	return (socketFd);
}

// setters

void Server::setIp(const std::string &ip)
{
	this->ip = ip;
}

void Server::setPort(unsigned int port)
{
	this->port = port;
}

void Server::setServerName(const std::string &serverName)
{
	this->serverName = serverName;
}

void Server::setSocketFd(int socketFd)
{
	this->socketFd = socketFd;
}

/*
	server가 가진 option목록 출력 후 location을 순환하며 printitem() 호출
*/
void Server::printItem()
{
	std::cout << "============== server option print ==============" << std::endl;
	for (std::map<std::string, std::string>::iterator lit = option.begin(); lit != option.end(); lit++)
	{
		std::cout << "save : {" << lit->first << "} {" << lit->second << "}" << std::endl;
	}
	for (std::vector<Location>::iterator it = location.begin(); it != location.end(); it++)
		it->printItem();
	
	std::cout << "==================================================" << std::endl;
}

/*
 * 입력 받은 문자열을 띄어쓰기, 탭 단위로 나눠서 std::vector로 리턴
 * @param const std::string &str : 띄어쓰기로 나눌 문자열
 * @return std::vector<std::string> & : 문자열을 나눈 결과
 */
std::vector<std::string> Server::splitSpaces(const std::string &str)
{
	std::vector<std::string>	result;
	char	*cur = const_cast<char *>(str.c_str());
	char	*prev = const_cast<char *>(cur);

	// NULL 만날 때까지 반복
	while (*cur)
	{
		if (ft_isblank(*cur))
		{
			result.push_back(str.substr(prev - str.c_str(), cur - prev));
			// *cur가 0이면 멈춰야 함, *cur가 space이면 다음으로 가야함
			while (*cur != '\0' && (*cur == ' ' || *cur == '\t'))
				cur++;
			prev = cur;
		}
		else
			cur++;
	}
	// 루프 다 돌고 나서 안 들어간 값이 있으면 추가
	if (cur != prev)
		result.push_back(str.substr(prev - str.c_str(), cur - prev));
	return (result);
}
