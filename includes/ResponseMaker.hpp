#ifndef RESPONSEMAKER_HPP
# define RESPONSEMAKER_HPP

# include "webserv.h"
# include "Response.hpp"
# include "Request.hpp"
# include "Location.hpp"
# include "Server.hpp"

/*
 * 리스폰스를 만들어주는 추상 클래스
 */
class ResponseMaker
{
	protected:
		Request request;									// 처리할 리퀘스트
		Server server;										// 요청 받은 서버
		Location location;									// 리퀘스트에 대응되는 로케이션 정보
		std::map<std::string, std::string> responseHeader;	// 돌려줄 리스폰스의 헤더
		std::map<std::string, std::string> mimeType;		// 리스폰스의 mimeType

		ResponseMaker();

	public:
		ResponseMaker(const ResponseMaker&);
		ResponseMaker(const Request&, const Server&, const Location&);
		ResponseMaker &operator=(const ResponseMaker&);
		virtual ~ResponseMaker();

		void		addResponseHeader(std::string, std::string);
		std::string getFormatTime(const struct tm*);
		void		addDateHeader(void);
		void		addServerHeader(void);
		void		addContentTypeHeader(std::string);
		void		addContentLengthHeader(int);

		std::string makeHTMLPage(std::string);
		Response	makeErrorResponse(int, std::string version = "HTTP/1.1");

		int 		checkPath(std::string path);
		void		updateErrorStatus(int clientFd, int statusCode);
};

#endif
