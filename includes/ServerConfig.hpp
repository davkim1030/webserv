#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

# include <vector>
# include <map>
# include <string.h>

class Server;

/*
	config 파일의 정보를 저장하고 가지고 다닐 클래스
	@member variable -> option : 기본 config 저장
	@member variable -> server : server 블럭이 저장된 vector 컨테이너
	@member variable -> instance : 자기 자신을 가지고 다닐 instance -> singleton pattern
*/
class ServerConfig
{
	private:
		std::map<std::string, std::string> option;
		std::vector<Server> server;
		static ServerConfig* instance;

		// singleton private default constructor
		ServerConfig() {}
	public:
		ServerConfig(ServerConfig const &co);
		ServerConfig &operator=(ServerConfig const &co);
		~ServerConfig();

		static ServerConfig*	getInstance();
		std::vector<Server>	const	&getServers();

		int		checkLine(std::string line);
		void	configParse(std::string line);
		int		openfile(int argc, char *path);
		void	saveConfig(int argc, char *path);

		void	printItem();

};

#endif
