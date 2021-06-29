#ifndef CGIWRITER_HPP
# define CGIWRITER_HPP

# include "webserv.h"
# include "IoObject.hpp"

/*
 * cgi를 처리하는 객체
 */
class CgiWriter : public IoObject
{
	private:
		int		clientFd;	// cgi를 호출한 리퀘스트 클라이언트 fd
		int		pos;		// 클라이언트 바디를 읽는 위치

	public:
		CgiWriter();
		CgiWriter(int fd, int clientFd);
		CgiWriter(const CgiWriter &cgi);
		CgiWriter &operator=(const CgiWriter &cgi);
		~CgiWriter();

		IoObject	*clone();
		int			getClientFd();
		int			getPos();

		void		setPos(int pos);
};
#endif
