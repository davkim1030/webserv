#ifndef SERVER_HPP
#define SERVER_HPP

# include <vector>
# include <map>

class Location;

class Server
{
	private:
		std::map<std::string, std::string> option;
		std::vector<Location> location;


	public:
		//std::string 형태로 인자 받아서 바로 파싱해서 저장하자
		Server();
		Server(const std::string &str);
		Server(Server const &serv);
		Server &operator=(Server const &co);
		~Server();
};


#endif