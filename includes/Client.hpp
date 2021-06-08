#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "webserv.h"
# include "Request.hpp"
# include "Response.hpp"

/*
 * 클라이언트의 상태를 나타내는 enum
 */
enum Status
{
	REQUEST_RECEIVING,	// 리퀘스트를 수신중
	RESPONSE_READY		// 리스폰스를 줄 준비가 됨
};

/*
 * 서버에 연결된 하나의 클라이언트
 */
class Client
{
	private:
		Status			status;			// 현재 상태
		int				serverSocketFd;	// 클라이언트가 연결될 서버 소켓의 fd
		int				socketFd;		// 클라이언트에 할당된 소켓 fd
		Request			request;		// 처리할 리퀘스트
		Response		response;		// 돌려줄 리스폰스
		long long		remainBody;		// 남은 바디 길이
		unsigned long	lastReqMs;		// 마지막으로 통신한 시간

	public:
		Client();
		Client(int serverSocketFd, int socketFd);
		~Client();

		// setters
		void		setSocketFd(int socketFd);
		void		setServerSocketFd(int serverSocketFd);
		void		setStatus(Status status);
		void		setRemainBody(long long remainBody);
		void		setLastReqMs(unsigned long lastReqMs);

		// getters
		int			getSocketFd();
		int			getServerSocketFd();
		Request		&getRequest();
		Response	&getResponse();
		Status		getStatus();
		long long	getRemainBody();
		unsigned long	getLastReqMs();
};
#endif
