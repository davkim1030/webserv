#include "webserv.h"
#include "Location.hpp"

Location::Location() {}

Location::Location(Location const &lo) : path(lo.path), option(lo.option)
{}

Location &Location::operator=(Location const &lo)
{
	if (this != &lo)
	{
		option = lo.option;
		path = lo.path;
	}
	return *this;
}

Location::~Location() {}

/*
	line을 받아 key value 형태로 저장
	@param -> std::string line : 중괄호 내부의 location이 가져야 할 옵션을 가진 문자열
*/
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

/*
	location 뒤로 붙은 path를 저장해주는 함수
	location /path { 의 형태로 들어오고 여기서 "/path" 만 저장
	@param -> std::string line -> 파싱에 쓰일 문자열
*/
void Location::saveLocation(std::string line)
{
	line = line.substr(0, line.find('{'));
	char *cLine = std::strtok((char *)line.c_str(), " \t");
	if (cLine == NULL)
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
	path = std::string(line);
}

/*
	Server에서 만든 location블럭을 받아 파싱 및 생성해주는 생성자
	줄이 location으로 시작하면 saveLocation 함수로 전달
	그 외에는 configParse 함수로 줄 전달
	@param -> std::list<std::string> line -> 파싱에 쓰일 location 저장한 list 개행 단위로 잘라 한 노드로 저장되어있음
*/
Location::Location(std::list<std::string> &line)
{
	for (std::list<std::string>::iterator it = line.begin(); it != line.end(); it++)
	{
		if (it->find("location") != std::string::npos)
			saveLocation(*it);
		else
			configParse(*it);
	}
}

/*
	location 멤버변수 path 반환
*/
std::string Location::getPath() const
{
	return path;
}

/*
	location 멤버변수 option[key] 값을 반환
	@param -> key -> option에 저장된 key와 매칭되는 value를 반환
*/
std::string Location::getOption(std::string const &key)
{	
	return option[key];
}

/*
	location의 이름 path 출력 후 내부 option 순환하며 모두 출력
*/
void Location::printItem()
{
	std::cout << "-----------location {" << path << "} option print----------" << std::endl;
	for (std::map<std::string, std::string>::iterator bit = option.begin(); bit != option.end(); bit++)
		std::cout << "option : {" << bit->first << "} {" << bit->second << "}" << std::endl;
}
