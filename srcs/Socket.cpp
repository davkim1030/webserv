#include "Socket.hpp"

Socket::Socket(int p = 80) : serverSD(-1), clientSD(-1), port(p) {}

const char *Socket::BindFailed::what() const throw()
{
	return "Error: bind failed";
}

const char *Socket::ListenFailed::what() const throw()
{
	return "Error: listen failed";
}

Socket::~Socket()
{
	// default
}

void Socket::initServer(int socketNum)
{
	ft_memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htons(INADDR_ANY);
	serverAddr.sin_port = htons(port);
	try
	{
		if (bind(serverSD, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) == 1)
			throw Socket::BindFailed();
		if (listen(serverSD, socketNum) == -1)
			throw Socket::ListenFailed();	
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}	
}
