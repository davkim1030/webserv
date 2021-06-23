#include "ResponseHandler.hpp"

ResponseHandler::ResponseHandler(const Request& request, const Location& location, int clientFd):
request(request), location(location), clientFd(clientFd){
	this->resourcePath = parseResourcePath(request.getUri());
};

ResponseHandler::ResponseHandler(const ResponseHandler& other)
{
	this->resourcePath = other.resourcePath;
	this->location = other.location;
}

ResponseHandler &ResponseHandler::operator=(const ResponseHandler& other)
{
	this->resourcePath = other.resourcePath;
	this->location = other.location;
}

ResponseHandler::~ResponseHandler()
{}

int ResponseHandler::checkAllowMethod(void)
{
	if (!location.getOption("allow_method").empty() && request.getMethod() != "GET" && request.getMethod() != "HEAD")
	{
		std::string allow = location.getOption("allow_method");
		if (allow.find(request.getMethod()) == std::string::npos)
			return METHOD_NOT_ALLOWED;
	}
	return CHECK_SUCCES;
}

/*
* Request URI에서 Location->path를 찾아, root 값으로 치환해줍니다.
* @param Request URI
* @return PATH가 ROOT로 치환된 URI
*/
std::string ResponseHandler::parseResourcePath(std::string uri)
{
	std::string path = this->location.getPath();
	if (*path.rbegin() != '/')
			path += '/';
	std::string root = this->location.getOption("root");
	size_t path_pos = uri.find_first_of(path);
	uri.replace(path_pos, path.length(), root);
	return uri;
}

/*
 * 경로가 존재하는지 / 파일인지 / 경로인지 판별합니다.
 * @param 체크할 경로
 * @return 경로가 존재하지 않을 경우 0 반환, 파일일 경우 1 반환, 디렉토리일경우 2 반환
 */
int ResponseMaker::checkPath(std::string path)
{
	struct stat buffer;

	int exist = stat(path.c_str(), &buffer);
	if (exist == 0)
	{
		if (S_ISREG(buffer.st_mode))
			return (ISFILE);
		else if (S_ISDIR(buffer.st_mode))
			return (ISDIR);
	}
	return (NOT_FOUND);
}

int ResponseHandler::tryToOpen(int openFlag)
{
	if (checkAllowMethod() == METHOD_NOT_ALLOWED)
		return (METHOD_NOT_ALLOWED);
	if (checkPath(this->resourcePath) == NOT_FOUND && request.getMethod() != "PUT" && request.getMethod() != "POST")
		return (NOT_FOUND);
	if ((this->fd = open(this->resourcePath.c_str(), openFlag)) < 0)
		return (SERVER_ERR);
	return (CHECK_SUCCES);
}

int ResponseHandler::tryToRead(void)
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

int ResponseHandler::tryToWrite(void)
{
	int status;

	if ((status = tryToOpen(O_WRONLY)) != CHECK_SUCCES)
		return (status);
	if (fstat(this->fd, &this->sb) < 0)
	{
		close(fd);
		return (SERVER_ERR);
	}
	if (checkPath(this->resourcePath) == ISDIR)
		return FORBIDDEN;
	return CHECK_SUCCES;
}

//fd를 fd풀에 등록 후 읽기 fdset flag 설정
void ResponseHandler::setReadFlag()
{
	Socket::getInstance()->getPool[fd] = new Resource(fd, clientFd);
	Socket::getInstance()->updateFds(fd, FD_READ);
}

//fd를 쓰기 fdset flag 설정

void ResponseHandler::setWriteFlag()
{
	Socket::getInstance()->getPool[fd] = new Resource(fd, clientFd);
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
int ResponseHandler::checkGetMethodIndex(void)
{
	if (checkPath(this->resourcePath) == ISDIR)
	{
		if (this->resourcePath[this->resourcePath.length() - 1] != '/')
			this->resourcePath += '/';

		if (!location.getOption("index").empty())
		{
			bool indexFileFlag = false;
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
			if (indexFileFlag == false && location.getOption("autoindex") == "on")
				return (CHECK_SUCCES);
			if (checkPath(this->resourcePath) == NOT_FOUND || checkPath(this->resourcePath) == ISDIR)
				return (NOT_FOUND);
		}
	}
	return (CHECK_SUCCES);
}