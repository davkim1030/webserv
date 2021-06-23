#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "IoObject.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "Server.hpp"

/*
 * 서버에 연결된 하나의 클라이언트
 */
class Client : public IoObject
{
	private:
		int				serverSocketFd;	// 클라이언트가 연결될 서버 소켓의 fd
		Request			request;		// 리퀘스트 데이터
		Response		response;		// 돌려줄 리스폰스
		size_t			pos;		// 남은 바디 길이
		unsigned long	lastReqMs;		// 마지막으로 통신한 시간

	public:
		Client();
		Client(int serverSocketFd, int fd);
		~Client();
		Client(const Client &other);

		Client &operator=(const Client &toher);

		// setters
		void		setServerSocketFd(int serverSocketFd);
		void		setStatus(Status status);
		void		setPos(size_t pos);
		void		setLastReqMs(unsigned long lastReqMs);
		void		setResponse(const Response &response);

		// getters
		Server		getServer();
		int			getServerSocketFd();
		Request		&getRequest();
		Response	&getResponse();
		Status		getStatus();
		size_t		getPos();
		unsigned long	getLastReqMs();

		// Inherited Functions
		IoObject	*clone();

		// member methods
		bool	headerParsable();
};
#endif
