#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

# include <vector>
# include <map>
# include <string.h>

class Server;

class ServerConfig
{
	private:
		std::map<std::string, std::string> option; // 기본 option 저장
		std::vector<Server> server;


	public:
		ServerConfig() {}
		ServerConfig(ServerConfig const &co);
		ServerConfig &operator=(ServerConfig const &co);
		~ServerConfig();

		int checkLine(std::string line);
		

		// option 파일의 option 저장
		void configParse(std::string line);

		void saveConfig(int fd);

};

#endif