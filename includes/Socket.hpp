#ifndef SOCKET_HPP
# define SOCKET_HPP

# include "webserv.h"
# include "Client.hpp"
# include "Server.hpp"
# include "Resource.hpp"
# include "ServerConfig.hpp"
# include "Location.hpp"
# include "Exception.hpp"
# include <climits>

# define IO_BUFFER_SIZE USHRT_MAX	// 버퍼가 한 번에 읽을 사이즈

// fd_set에 플래그를 세울 때 쓰는 타입
enum FdType
{
	FD_READ,
	FD_WRITE,
	FD_EXCEPT
};

/*
 * 소켓 통신 서버
 */
class Socket
{
	private:
		int		fdMax;	// 처리할 최대 fd 값
		fd_set	rfds;	// 읽기 fd set
		fd_set	wfds;	// 쓰기 fd set
		fd_set	efds;	// 예외 fd set
		// static const int IO_BUFFER_SIZE = USHRT_MAX;	// 위로 이동
		static Socket	*instance;	// 프로그램에서 유일하게 사용할 소켓 인스턴스
		ServerConfig	serverConfig;	// config 파일에서 읽을 서버 설정 값들

		std::vector<IoObject *>	pool;	// Server, Client, Resource, CGI 객체들의 IO를 담당할 벡터

		Socket(Socket const &so);
		Socket &operator=(Socket const &so);

		void clearConnectedSocket(int fd);
		bool isCgi(std::string rawUri, Location location);
		void prepareResponse(Client *client);

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

		void runServer(struct timeval timeout);
		static Socket *getInstance();
		void initServer(int argc, char *argv);
		void updateFdMax();
		void updateFds(int fd, FdType fdType);
		void handleError(int clientFd, int selfFd,  int statusCode);


		void	doReadFdset(int fd);

		std::vector<IoObject *> &getPool();
};
#endif
