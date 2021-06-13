#ifndef SOCKET_HPP
# define SOCKET_HPP

# include "webserv.h"
# include "Client.hpp"
# include "Server.hpp"
# include "ServerConfig.hpp"
# include "Location.hpp"
# include "ResponseHandler.hpp"
# include "Exception.hpp"

/*
 * 소켓 통신 서버
 */
class Socket
{
	private:
		fd_set	rfds;	// 읽기 fd set
		fd_set	wfds;	// 쓰기 fd set
		fd_set	efds;	// 예외 fd set
		int		fdMax;	// 처리할 최대 fd 값

		std::map<int, Server> servers;	// 프로그램에서 처리할 서버들 정보
		std::map<int, Client> clients;	// 프로그램에 연결된 클라이언트 정보

		Socket(Socket const &so);
		Socket &operator=(Socket const &so);

		void clearConnectedSocket(int fd);
		void updateFdMax();

		class SocketException : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};
		class BindException : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};
		
		class ListenException : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};

		class AcceptException : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};

		class SelectException : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};

	public:
		Socket();
		~Socket();

		void initServer(int socketCnt);
		void runServer(struct timeval timeout, unsigned int bufferSize);
};
#endif
