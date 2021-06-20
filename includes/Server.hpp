#ifndef SERVER_HPP
# define SERVER_HPP

# include "IoObject.hpp"
# include <vector>
# include <map>

class Location;

/*
	서버 클래스
	관련 데이터 저장 및 서버 구동
*/
class Server : public IoObject
{
	private:
		std::map<std::string, std::string> option;	// 서버가 가질 기본 정보 저장
		std::vector<Location>		location;		// location 블럭이 저장된 vector 컨테이너
		std::string					ip;				// 서버의 ip 주소
		unsigned int 				port;			// 서버의 포트
		std::string					serverName;		// 서버 이름

	public:
		// std::string 형태로 인자 받아서 바로 파싱해서 저장하자
		Server();
		Server(std::list<std::string> &save);
		Server(Server const &serv);
		Server &operator=(Server const &co);
		~Server();

		// Getters
		std::vector<Location> getLocationVector();
		Location *getLocation(std::string const& path);
		std::string getOption(std::string const &key);
		const std::string	&getIp() const;
		unsigned int		getPort() const;
		const std::string	&getServerName() const;

		// Setters
		void	setIp(const std::string &ip);
		void	setPort(unsigned int port);
		void	setServerName(const std::string &serverName);

		// Member Functions
        void	configParse(std::string line);
		void	printItem();
        int		checkLine(std::string line);
		int		sliceOptions();

		// Inherited Functions
		void	doRead();
		void	doWrite();
};
#endif
