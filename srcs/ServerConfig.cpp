#include "webserv.h"
#include "Server.hpp"
#include "Exception.hpp"
#include "ServerConfig.hpp"
#include "Exception.hpp"

ServerConfig::ServerConfig()
{}

// copy constructor
ServerConfig::ServerConfig(ServerConfig const &co) : option(co.option), server(co.server) {}

// operator=
ServerConfig &ServerConfig::operator=(ServerConfig const &co)
{
	if (this != &co)
	{
		option = co.option;
		server = co.server;
	}
	return *this;
}

// destructor
ServerConfig::~ServerConfig() {}

/*
	private 멤버 변수 server 반환 getter
*/
std::vector<Server> &ServerConfig::getServers()
{
	return server;
}

/*
	줄이 공백만 존재하는지 체크
	@param -> std::string line : config 파일 내부의 한 줄
*/
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

/*
	line을 받아서 key : value 형태로 저장
	ex) user nginx;
	ex) key : user | value : nginx
	@param -> std::string line : server블럭에 묶여있지 않은 프로그램의 옵션을 가진 문자열
*/
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

/*
	file path를 받아서 config file open 후 fd 반환
	파일 open에 실패할 경우 throw FileDoesNotExist
	@param -> int argc : 2이면 path를 open 아닐 경우 default path를 open
	@param -> char *path : argc가 2일 때 open할 파일의 경로
*/
int	ServerConfig::openfile(int argc, char *path)
{
	int fd;
	if (argc == 2)
		fd = open(path, O_RDONLY);
	else
		fd = open(CONFIG_PATH, O_RDONLY);
	if (fd == -1)
		throw FileDoesNotExistException();
	return fd;
}

/*
	컨피그 파일 open 및 정보 저장
	get_next_line을 이용해 파일을 읽어 파싱
	server로 묶인 블럭이 나오면 멤버 변수 server에 저장
	그 외에는 멤버 변수 option으로 저장
	파일 내부의 중괄호가 올바르게 있지 않으면 throw WrongFileFormat
	자세한 내용은 config파일에 기재
	@param -> int argc : openfile 에 넘겨줄 프로그램 인자 argc
	@param -> char *path : openfile 에 넘겨줄 프로그램 인자 argv[1]
*/
void ServerConfig::saveConfig(int argc, char *path)
{
	int parenFlag = 0;
	char *line;

	int fd = openfile(argc, path);
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
	if (line != NULL)
		free(line);
	close(fd);
	if (checkDuplicatePort() < 0)
		parenFlag--;
	if (parenFlag != 0)
		throw WrongFileFormatException();
}

/*
	각 서버들 내에 중복되는 port가 있는지 검사해주는 함수
*/
int ServerConfig::checkDuplicatePort()
{
	if (server.empty() == true)
		return -1;
	for (std::vector<Server>::iterator sit = server.begin(); sit != --server.end(); sit++)
	{
		std::cout << sit->getServerName() << ":" << sit->getPort() << std::endl;;
		for (std::vector<Server>::iterator nextSit = sit; nextSit != server.end(); )
		{
			nextSit++;
			std::cout << nextSit->getServerName() << ":"<< nextSit->getPort() << "/";
			if (sit->getPort() == nextSit->getPort())
				return -1;
		}
		std::cout << std::endl;
	}

	return 0;
}

/*
	프로그램이 가져야 할 config 출력 후 server를 순환하며 printItem() 호출
*/
void ServerConfig::printItem()
{
	std::map<std::string, std::string>::iterator it;
	std::cout << "===========Server config=========" << std::endl;
	for (it = option.begin(); it != option.end(); it++)
		std::cout << "config : {" << it->first << "} {" << it->second << "}" << std::endl;

	for (std::vector<Server>::iterator sit = server.begin(); sit != server.end(); sit++)
		sit->printItem();
}

std::string ServerConfig::getOption(std::string const &key)
{
	if (option.count(key) == 0)
		return "";
	return option[key];
}
