#include "ResourceHandler.hpp"

/* 노말리스폰스랑 합병 */
// ResourceHandler::ResourceHandler(){}

ResourceHandler::ResourceHandler(const Request& request, const Server& server, const Location& location, int clientFd)
: ResponseMaker(request, server, location), clientFd(clientFd), exist(-1), autoIndex(false){
	this->resourcePath = parseResourcePath(request.getUri());
}

ResourceHandler::ResourceHandler(const ResourceHandler& other)
: ResponseMaker(other)
{
	if (this == &other)
		return ;
	this->resourcePath = other.resourcePath;
	this->fd = other.fd;
	this->sb = other.sb;
	this->clientFd = other.clientFd;
	this->exist = other.exist;
	this->autoIndex = other.autoIndex;
}

ResourceHandler &ResourceHandler::operator=(const ResourceHandler& other)
{
	if (this == &other)
		return *this;
	this->resourcePath = other.resourcePath;
	this->location = other.location;
	this->request = other.request;
	this->fd = other.fd;
	this->sb = other.sb;
	this->clientFd = other.clientFd;
	this->exist = other.exist;
	this->autoIndex = other.autoIndex;
	return *this;
}

ResourceHandler::~ResourceHandler()
{}

bool ResourceHandler::checkAllowMethod(void)
{
	if (!location.getOption("allow_method").empty() && request.getMethod() != "GET" && request.getMethod() != "HEAD")
	{
		std::string allow = location.getOption("allow_method");
		if (allow.find(request.getMethod()) == std::string::npos)
		{
			updateErrorStatus(clientFd, METHOD_NOT_ALLOWED);
			return (false);
		}
	}
	return (true);
}

/*
* Request URI에서 Location->path를 찾아, root 값으로 치환해줍니다.
* @param Request URI
* @return PATH가 ROOT로 치환된 URI
*/
std::string ResourceHandler::parseResourcePath(std::string uri)
{
	std::string path = this->location.getPath();
	if (*path.rbegin() != '/')
			path += '/';
	std::string root = this->location.getOption("root");
	size_t path_pos = uri.find_first_of(path);
	uri.replace(path_pos, path.length(), root);
	return uri;
}

int ResourceHandler::tryToOpen(int openFlag)
{
	if (checkPath(this->resourcePath) == NOT_FOUND && request.getMethod() != "PUT" && request.getMethod() != "POST")
		return (NOT_FOUND);
	if ((this->fd = open(this->resourcePath.c_str(), openFlag)) < 0)
		return (SERVER_ERR);
	return (CHECK_SUCCES);
}

int ResourceHandler::tryToRead(void)
{
	int status;

	if ((status = tryToOpen(O_RDONLY)) != CHECK_SUCCES)
		return (status);
	if (fstat(this->fd, &this->sb) < 0)
	{
		close(fd);
		return (SERVER_ERR);
	}
	return CHECK_SUCCES;
}

int ResourceHandler::tryToPost()
{
	this->exist = checkPath(this->resourcePath);
	switch (this->exist)
	{
		case NOT_FOUND :
		{
			if ((fd = open(this->resourcePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0)
				return (SERVER_ERR);
			if (fstat(this->fd, &this->sb) < 0)
			{
				close(fd);
				return (SERVER_ERR);
			}
			return CHECK_SUCCES;
		}
		case ISFILE :
		{
			if ((fd = open(this->resourcePath.c_str(), O_WRONLY | O_APPEND)) < 0)
				return (SERVER_ERR);
			if (fstat(this->fd, &this->sb) < 0)
			{
				close(fd);
				return (SERVER_ERR);
			}
			return CHECK_SUCCES;
		}
		default :
			return FORBIDDEN;
	}
}

int ResourceHandler::tryToPut()
{
	this->exist = checkPath(this->resourcePath);
	switch (this->exist)
	{
		case NOT_FOUND :
		{
			if ((fd = open(this->resourcePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0)
				return (SERVER_ERR);
			if (fstat(this->fd, &this->sb) < 0)
			{
				close(fd);
				return (SERVER_ERR);
			}
			return CHECK_SUCCES;
		}
		case ISFILE :
		{
			if ((fd = open(this->resourcePath.c_str(), O_WRONLY | O_TRUNC)) < 0)
				return (SERVER_ERR);
			if (fstat(this->fd, &this->sb) < 0)
			{
				close(fd);
				return (SERVER_ERR);
			}
			return CHECK_SUCCES;
		}
		default :
			return FORBIDDEN;
	}
}

int ResourceHandler::wasExist(void)
{
	return (this->exist);
};


//fd를 fd풀에 등록 후 읽기 fdset flag 설정
void ResourceHandler::setReadFlag()
{
	Socket::getInstance()->getPool()[fd] = new Resource(fd, clientFd);
	Socket::getInstance()->updateFds(fd, FD_READ);
}

//fd를 쓰기 fdset flag 설정

void ResourceHandler::setWriteFlag()
{
	Socket::getInstance()->getPool()[fd] = new Resource(fd, clientFd);
	Socket::getInstance()->updateFds(fd, FD_WRITE);
}

//Response string을 받아 fdset에 쓰기 flag 설정
// void ResponseHandler::string(std::string )
// {
// 	Socket::getInstance()->getpool[fd] = new Resource(fd, clientFd);
// 	Socket::getInsance()->updateFds(fd, FD_WRITE);
// }

//fd 내용이 다 읽힌 게 확인되면, fd를 pool에서 삭제

//GET의 리소스파일 유효성 검사
int ResourceHandler::checkGetMethodIndex(void)
{
	if (checkPath(this->resourcePath) == ISDIR)
	{
		if (this->resourcePath[this->resourcePath.length() - 1] != '/')
			this->resourcePath += '/';

		bool indexFileFlag = false;
		if (!location.getOption("index").empty())
		{
			std::vector<std::string> indexFile = splitSpaces(location.getOption("index"));
			for (std::vector<std::string>::iterator iter = indexFile.begin();
					iter != indexFile.end(); iter++)
			{
				struct stat buffer;
				if (stat((this->resourcePath + *iter).c_str(), &buffer) == 0)
				{
					this->resourcePath = this->resourcePath + *iter;
					indexFileFlag = true;
					break ;
				}
			}
		}
		if (indexFileFlag == false && location.getOption("autoindex") == "on")
		{	
			autoIndex = true;
			return (CHECK_SUCCES);
		}
		if (checkPath(this->resourcePath) == NOT_FOUND || checkPath(this->resourcePath) == ISDIR)
			return (NOT_FOUND);
	}
	return (CHECK_SUCCES);
}

//Request가 Resource를 필요로하는 타입인지 확인
bool ResourceHandler::CheckResourceType(void)
{
	int stat;	
	if (this->request.getMethod() == "GET" || this->request.getMethod() == "HEAD")
	{
		if ((stat = checkGetMethodIndex()) != CHECK_SUCCES)
		{
			updateErrorStatus(clientFd, stat);
			return false;
		}
		if (this->autoIndex == false)
		{
			if ((stat = tryToRead()) != CHECK_SUCCES)
			{
				updateErrorStatus(clientFd, stat);
				return false;
			}
			setReadFlag();
		}
		return true;
	}
	else if (request.getMethod() == "POST")
	{
		if ((stat = tryToPost()) != CHECK_SUCCES)
		{
			updateErrorStatus(clientFd, stat);
			return false;
		}
		setWriteFlag();
		return true;
	}
	else if (request.getMethod() == "PUT")
	{
		if ((stat = tryToPut()) != CHECK_SUCCES)
		{
			updateErrorStatus(clientFd, stat);
			return false;
		}
		setWriteFlag();
		return true;
	}
	return true;
}

bool ResourceHandler::isAutoIndex(void)
{
	return this->autoIndex;
}