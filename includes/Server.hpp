#ifndef SERVER_HPP
#define SERVER_HPP

# include <vector>
# include <map>

class Location;

/*
	server 블럭 저장 클래스
	@member variable -> option : server가 가질 기본 정보 저장
	@member variable -> location : location 블럭이 저장된 vector 컨테이너
*/
class Server
{
	private:
		std::map<std::string, std::string> option;
		std::vector<Location> location;


	public:
		//std::string 형태로 인자 받아서 바로 파싱해서 저장하자
		Server();
		Server(std::list<std::string> &save);
		Server(Server const &serv);
		Server &operator=(Server const &co);
		~Server();

		std::vector<Location> getLocationVector();
		Location *getLocation(std::string const& path);

        int checkLine(std::string line);
        void configParse(std::string line);

		void printItem();

		std::string getOption(std::string const &key);
};


#endif
