#include "CgiResource.hpp"

CgiResource::CgiResource() : IoObject(CGI), pid(-1), lseekFlag(false), clientFd(-1), fileSize(0)
{}

CgiResource::CgiResource(int fd, pid_t pid, int clientFd)
: IoObject(fd, "", REQUEST_RECEIVING_HEADER, CGI_RESOURCE), pid(pid), lseekFlag(false), clientFd(clientFd), fileSize(0)
{}

CgiResource::CgiResource(const CgiResource &cgi)
: IoObject(cgi), pid(cgi.pid), lseekFlag(cgi.lseekFlag), clientFd(cgi.clientFd), fileSize(cgi.fileSize)
{}

CgiResource &CgiResource::operator=(const CgiResource &cgi)
{
	if (this != &cgi)
	{
		IoObject::operator=(cgi);
		pid = cgi.pid;
		lseekFlag = cgi.lseekFlag;
		clientFd = cgi.clientFd;
		fileSize = cgi.fileSize;
	}
	return *this;
}

CgiResource::~CgiResource()
{}

IoObject *CgiResource::clone()
{
	IoObject *tmp = new CgiResource(*this);
	return tmp;
}

pid_t   CgiResource::getPid()
{
	return this->pid;
}

void	CgiResource::setLseek()
{
	this->lseekFlag = true;
}

bool	CgiResource::isLseek()
{
	return lseekFlag;
}

int		CgiResource::getClientFd()
{
	return this->clientFd;
}

int		CgiResource::getFileSize()
{
	return this->fileSize;
}

void	CgiResource::setFileSize(int fileSize)
{
	this->fileSize = fileSize;
}
