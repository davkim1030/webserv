#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "webserv.h"

class Socket
{
	private:
		int serverSD; // 서버 socket discriptor
		int clientSD; // 클라이언트 socket discriptor
		int port; // 포워딩 해야 할 포트 번호
		struct sockaddr_in serverAddr; // 서버 소켓 정보 저장용 구조체
		struct sockaddr_in clientAddr; // 클라이언트 소켓 정보 저장용 구조체

		Socket(Socket const &so);
		Socket &operator=(Socket const &so);

		class BindFailed : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};

		class ListenFailed : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};


	public:
		Socket(int p = 80);
		~Socket();

		void initServer(int);
};

#endif