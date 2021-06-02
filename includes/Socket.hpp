#ifndef SOCKET_HPP
# define SOCKET_HPP

# include "webserv.h"

/*
 * 소켓 통신 서버
 */
class Socket
{
	private:
		int serverSd; // 서버 socket descriptor
		int clientSd; // 클라이언트 socket descriptor
		int port; // 포워딩 해야 할 포트 번호
		int socketCnt;	// 오픈할 소켓의 개수
		struct sockaddr_in serverAddr; // 서버 소켓 정보 저장용 구조체
		struct sockaddr_in clientAddr; // 클라이언트 소켓 정보 저장용 구조체

		Socket(Socket const &so);
		Socket &operator=(Socket const &so);

		void operateIo(int maxSize, fd_set rfds, fd_set wfds, fd_set efds);

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

	public:
		// Constructors & Destructors

		Socket(int p = 80);
		~Socket();

		void initServer(int socketCnt);
		void runServer();
};
#endif
