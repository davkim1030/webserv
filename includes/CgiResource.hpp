#ifndef CGIRESOURCE_HPP
# define CGIRESOURCE_HPP

# include "webserv.h"
# include "IoObject.hpp"

/*
 * cgi에서 만든 결과를 저장한 임시 리소스 파일 객체
 */
class CgiResource : public IoObject
{
	private:
		pid_t	pid;
		bool	lseekFlag;
		int		clientFd;
		int		fileSize;

	public:
		CgiResource();
		CgiResource(int fd, pid_t pid, int clientFd);
		CgiResource(const CgiResource &cgi);
		CgiResource &operator=(const CgiResource &cgi);
		~CgiResource();

		IoObject	*clone();
		
		pid_t		getPid();
		int			getClientFd();
		int			getFileSize();

		void		setLseek();
		void		setFileSize(int fileSize);

		bool		isLseek();
};

#endif
