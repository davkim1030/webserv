#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "IoObject.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "Server.hpp"
# include "webserv.h"

enum	ChunkedFlag
{
	LEN,
	BODY
};

enum	ReadStatus
{
	DISCONNECT,
	REQUEST_DONE,
	REQUEST_WAITING
};

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
		size_t			bodyLen; //chunked일 때 읽은 값의 인덱스를 저장
		ChunkedFlag		chunkedFlag;	// 읽는게 바디인지 숫자인지

		Location findLocation(Server server, std::string uri);

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
		void		setBodyLen(size_t chunkedIndex);
		void		setChunkedFlag(ChunkedFlag chunkedFlag);

		// getters
		Server		getServer();
		int			getServerSocketFd();
		Request		&getRequest();
		Response	&getResponse();
		Status		getStatus();
		size_t		getPos();
		unsigned long	getLastReqMs();
		size_t		getBodyLen();
		ChunkedFlag	getChunkedFlag();

		// Inherited Functions
		IoObject	*clone();

		// member methods
		bool	headerParsable();

		Status		readRequestMessage(std::vector<IoObject *>	&pool, int currentFd);

};
#endif
