#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <vector>
# include <map>
# include <string.h>

class Server;

/*
 * config 파일의 정보를 저장하고 가지고 다닐 클래스
 */
class 	ServerConfig
{
	private:
		std::map<std::string, std::string> option;	// 기본 config 저장
		std::vector<Server> server;					// server 블럭이 저장된 vector 컨테이너

	public:
		ServerConfig();
		ServerConfig(ServerConfig const &co);
		ServerConfig &operator=(ServerConfig const &co);
		~ServerConfig();

		std::vector<Server>		&getServers();

		int		checkLine(std::string line);
		void	configParse(std::string line);
		int		openfile(int argc, char *path);
		void	saveConfig(int argc, char *path);
		int		checkDuplicatePort();

		std::string getOption(std::string const &key);

		void	printItem();
};

#endif
