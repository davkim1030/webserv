#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "IoObject.hpp"
# include "Request.hpp"
# include "Response.hpp"

/*
 * 서버에 연결된 하나의 클라이언트
 */
class Client : public IoObject
{
	private:
		int				serverSocketFd;	// 클라이언트가 연결될 서버 소켓의 fd
		Request			request;		// 리퀘스트 데이터
		Response		response;		// 돌려줄 리스폰스
		long long		remainBody;		// 남은 바디 길이
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
		void		setRemainBody(long long remainBody);
		void		setLastReqMs(unsigned long lastReqMs);
		void		setResponse(const Response &response);

		// getters
		int			getServerSocketFd();
		Request		&getRequest();
		Response	&getResponse();
		Status		getStatus();
		long long	getRemainBody();
		unsigned long	getLastReqMs();

		// Inherited Functions
		void	doRead();
		void	doWrite();
		IoObject	*clone();

		// member methods
		bool	headerParsable();
		bool	bodyParsable();
};
#endif
