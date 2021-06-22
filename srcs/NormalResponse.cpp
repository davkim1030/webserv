#include "NormalResponse.hpp"
#include "Socket.hpp"

NormalResponse::NormalResponse(const Request& request, const Server& server, const Location& location)
: ResponseMaker(request, server, location)
{}

NormalResponse::NormalResponse(const NormalResponse &nor)
: ResponseMaker(nor)
{}

NormalResponse &NormalResponse::operator=(const NormalResponse &nor)
{
    if (this != &nor)
    {
        ResponseMaker::operator=(nor);
    }
    return *this;
}

NormalResponse::~NormalResponse()
{}
// ====================== Header ======================

/*
* 경로의 최신 수정시간을 헤더에 추가해주는 함수
* @param 최신 수정시간을 구할 경로
*/
void NormalResponse::addLastModifiedHeader(std::string path)
{
	struct stat statbuff;
	struct tm*	timeinfo;

	if (stat(path.c_str(), &statbuff) < 0)
		throw Response(500, responseHeader, makeHTMLPage("500"), request.getHttpVersion());
	timeinfo = localtime(&statbuff.st_mtime);
	addResponseHeader("Last-Modified", getFormatTime(timeinfo));
}

void NormalResponse::addContentLanguageHeader()
{
	addResponseHeader("Content-Language", "ko-KR");
}

void NormalResponse::addContentLocationHeader()
{
	addResponseHeader("Content-Location", this->resourcePath.substr(1));
}

void NormalResponse::addAllowHeader(std::string allow)
{
	addResponseHeader("Allow", allow);
}

void NormalResponse::addHostHeader()
{
	addResponseHeader("Host", resourcePath);
}

/*
* 입력 메소드와 헤더를 판단해 대응하는 응답값을 출력합니다.
* @param 파싱과 읽기를 완료한 resource string
* @return 응답값을 담은 Response
*/
Response NormalResponse::makeResponse(const std::string &resource)
{
	this->resource = resource;
	try{
		//location에서 uri를 찾지말고 uri에서 location을 찾아야합니다.
		if (!location.getOption("allow_method").empty() && request.getMethod() != "GET" && request.getMethod() != "HEAD")
		{
			std::string allow = location.getOption("allow_method");
			if (allow.find(request.getMethod()) == std::string::npos)
			{
				addAllowHeader(allow);
				throwErrorResponse(METHOD_NOT_ALLOWED, request.getHttpVersion());
			}
		}
		if (request.getMethod() == "TRACE")
			makeTraceResponse();
		else if (request.getMethod() == "OPTIONS")
			makeOptionResponse();
		else if (request.getMethod() == "CONNECT")
			makeConnectResponse();
		this->resourcePath = parseResourcePath(request.getUri());

		if (checkPath(this->resourcePath) == NOT_FOUND && request.getMethod() != "PUT" && request.getMethod() != "POST")
			throwErrorResponse(NOT_FOUND, request.getHttpVersion());
		if (request.getMethod() == "GET")
			makeGetResponse(0);
		if (request.getMethod() == "HEAD")
			makeHeadResponse();
		/*if (request.getMethod() == "POST")
			makePostResponse();
		if (request.getMethod() == "PUT")
			makePutResponse();*/
		if (request.getMethod() == "DELETE")
			makeDeleteResponse();
	}
	catch (Response &e)
	{
		return e;
	}

	responseHeader.clear();
	resourcePath.clear();
	throwErrorResponse(500, request.getHttpVersion());
	return Response(500, responseHeader, makeHTMLPage("500"), request.getHttpVersion());
}

/*
* Request URI에서 Location->path를 찾아, root 값으로 치환해줍니다.
* @param Request URI
* @return PATH가 ROOT로 치환된 URI
*/
std::string NormalResponse::parseResourcePath(std::string uri)
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
* GET 메소드의 Response를 생성합니다.
* 기본 헤더 : Date, Server
* 엔티티 헤더 : Last-Modified
* 컨텐츠 헤더 : Content-Language, Content-Length, Content-Location,	Content-Type
* 반환 Status-Code : 404(찾을 수 없음), 200(성공), 500(내부 서버 오류), 403(권한이 없음)
*/
void NormalResponse::makeGetResponse(int httpStatus)
{
	int fd;
	struct stat	sb;

	addDateHeader();
	addServerHeader();
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
			{
				addContentTypeHeader(".html");
				throw Response(200, this->responseHeader, request.getMethod() != "HEAD" ? makeAutoIndexPage(this->resourcePath) : "", request.getHttpVersion());
			}
			if (checkPath(this->resourcePath) == NOT_FOUND || checkPath(this->resourcePath) == ISDIR)
				throwErrorResponse(NOT_FOUND, request.getHttpVersion());
		}
	}
	if ((fd = open(this->resourcePath.c_str(), O_RDONLY)) < 0)
		throwErrorResponse(SERVER_ERR, request.getHttpVersion());
	if (fstat(fd, &sb) < 0)
	{
		close(fd);
		throwErrorResponse(SERVER_ERR, request.getHttpVersion());
	}
	int amsd; // TODO : ERROR케이스 아무거나 막 넣어둔거니 꼭 없애줘야함
	Resource *resource = new Resource(fd, amsd);
	Socket::getInstance()->getPool()[fd] = resource;
	// FD_SET(fd, Socket::getFdSet);
	Socket::getInstance()->updateFdMax();

	if (this->resource.size() < 0 || this->resource.empty())
		throwErrorResponse(SERVER_ERR, request.getHttpVersion());
	addContentTypeHeader(fileExtension(resourcePath.substr(1)));
	addContentLanguageHeader();
	addContentLocationHeader();
	addContentLengthHeader((int)sb.st_size);
	addLastModifiedHeader(resourcePath);
	if (httpStatus == HEAD_METHOD)
		throw Response(200, responseHeader, "", request.getHttpVersion());
	throw Response(200, responseHeader, this->resource, request.getHttpVersion());
}

/*
* HEAD 메소드의 Response를 생성합니다.
* allowed method인지 확인하고 이상이 없으면 makeGetResponse 함수를 호출한다.
*/
void NormalResponse::makeHeadResponse(void)
{
	if (server.getOption("allowed_method").find("HEAD") == std::string::npos)
	{
		addContentTypeHeader(".html");
		addDateHeader();
		addServerHeader();
		Response tmp(405, responseHeader, "", request.getHttpVersion());
		std::cout << "HEAD test\n" << tmp.getMessage() << std::endl;
		throw Response(405, responseHeader, "", request.getHttpVersion());
	}
	makeGetResponse(HEAD_METHOD);
}

/*
* 입력받은 파일의 확장자를 반환합니다.
*/
std::string NormalResponse::fileExtension(std::string resourcePath)
{
	if (resourcePath.find('.') != std::string::npos)
		return (resourcePath.substr(resourcePath.find('.')));
	return resourcePath;
}

/*
* 클라이언트가 서버에게 지정한 URL에 지정한 데이터를 저장할 것을 요청한다.
* 멱등성 X(데이터를 계속해서 새로 추가함.)
* RESPONSE
* 기본 헤더 : Date, Server
* 컨텐츠 헤더 :Content-Location
* 반환 Status-Code :
* 기존에 없는 것을 새로이 생성 - 201 (Created)
* 기존에 있는 것을 성공적으로 수정 - 200 (OK) 또는 204 (No Content) 응답
* 실패 : 500(SERVER ERR)
* 권한없음 : 403(forbidden)
*//*
void NormalResponse::makePostResponse(void)
{
	int fd;
	int pathType = checkPath(this->resourcePath);

	switch (pathType)
	{
		case NOT_FOUND :
		{
			if ((fd = open(this->resourcePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0)
				throwErrorResponse(SERVER_ERR, request.getHttpVersion());

			if (request.getHeader()["Transfer-Encoding"] != "chunked")
				write(fd, request.getRawBody().c_str(), ft_atoi(request.getHeader()["Content-Length"].c_str()));
			else
				write(fd, request.getRawBody().c_str(), request.getRawBody().length());
			close(fd);
			addContentLocationHeader();
			throw Response(201, responseHeader, "", request.getHttpVersion());
		}
		case ISFILE :
		{
			if ((fd = open(this->resourcePath.c_str(), O_WRONLY | O_APPEND )) < 0)
				throwErrorResponse(SERVER_ERR, request.getHttpVersion());
			if (request.getHeader()["Transfer-Encoding"] != "chunked")
				write(fd, request.getRawBody().c_str(), ft_atoi(request.getHeader()["Content-Length"].c_str()));
			else
				write(fd, request.getRawBody().c_str(), request.getRawBody().length());
			close(fd);
			addContentLocationHeader();
			throw Response(200, responseHeader, "", request.getHttpVersion());
		}
		default :
			throwErrorResponse(FORBIDDEN, request.getHttpVersion());
	}
}*/

/*
* 클라이언트가 서버에게 지정한 URL에 지정한 데이터를 저장하거나 대체할 것을 요청한다.
* 멱등성 O(데이터를 계속해서 새로 만듬. 기존에 데이터가 있으면 싹 지워버리고 다시씀.)
* RESPONSE
* 기본 헤더 : Date, Server
* 컨텐츠 헤더 :Content-Location
* 반환 Status-Code :
* 기존에 없는 것을 새로이 생성 - 201 (Created)
* 기존에 있는 것을 성공적으로 수정 - 200 (OK) 또는 204 (No Content) 응답
* 실패 : 500(SERVER ERR)
* 권한없음 : 403(forbidden)
*//*
void NormalResponse::makePutResponse(void)
{
	int fd;
	int pathType = checkPath(this->resourcePath);

	switch (pathType)
	{
		case NOT_FOUND :
		{
			if ((fd = open(this->resourcePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0)
				throwErrorResponse(SERVER_ERR, request.getHttpVersion());

			if (request.getHeader()["Transfer-Encoding"] != "chunked")
				write(fd, request.getRawBody().c_str(), ft_atoi(request.getHeader()["Content-Length"].c_str()));
			else
				write(fd, request.getRawBody().c_str(), request.getRawBody().length());
			close(fd);
			addContentLocationHeader();
			throw Response(201, responseHeader, "", request.getHttpVersion());
		}
		case ISFILE :
		{
			if ((fd = open(this->resourcePath.c_str(), O_WRONLY | O_TRUNC)) < 0)
				throwErrorResponse(SERVER_ERR, request.getHttpVersion());
			if (request.getHeader()["Transfer-Encoding"] != "chunked")
				write(fd, request.getRawBody().c_str(), ft_atoi(request.getHeader()["Content-Length"].c_str()));
			else
				write(fd, request.getRawBody().c_str(), request.getRawBody().length());
			close(fd);
			addContentLocationHeader();
			throw Response(200, responseHeader, "", request.getHttpVersion());
		}
		default :
			throwErrorResponse(FORBIDDEN, request.getHttpVersion());
	}
}*/

/*
* DELETE 메소드의 작업을 수행한다.
* 기본 헤더 : Date, Server
* 엔티티 헤더 : Allow
* 반환 Status-Code :
* 202 (Accepted) - 아마도 명령을 성공적으로 수행할 것 같으나 아직은 실행하지 않은 경우
* 204 (No Content) - 명령을 수행했고 더 이상 제공할 정보가 없는 경우
* 200 (OK) - 명령을 수행했고 응답 메시지가 이후의 상태를 설명하는 경우
* 404(찾을 수 없음)
* 403(권한이 없음)
*/
void NormalResponse::makeDeleteResponse(void)
{
	addDateHeader();
	addServerHeader();
	if (checkPath(this->resourcePath) == ISFILE)
	{
		unlink(this->resourcePath.c_str());
		if (!location.getOption("allow_method").empty())
		{
			std::string allow = location.getOption("allow_method");
			addAllowHeader(allow);
		}
		throw Response(200, responseHeader, makeHTMLPage("File deleted"), request.getHttpVersion());
	}
	throwErrorResponse(NOT_FOUND, request.getHttpVersion());
}

/*
* 클라이언트가 서버에게 송신한 요청을 반환한다.
*/
void NormalResponse::makeTraceResponse()
{
	addContentTypeHeader("message/http");
	throw Response(200, request.getHeader(), "", request.getHttpVersion());
}

/*
* 해당 URL에서 지원하는 요청 method 목록을 요청한다.
*/
void NormalResponse::makeOptionResponse()
{
	if (!location.getOption("allow_method").empty())
	{
		std::string allow = location.getOption("allow_method");
		addAllowHeader(allow);
	}
	throw Response(200, responseHeader, "", request.getHttpVersion());
}

/*
* 클라이언트가 특정 종류의 프록시 서버에게 연결을 요청한다.
*/
void NormalResponse::makeConnectResponse()
{
	addHostHeader();
	throw Response(200, responseHeader, "", request.getHttpVersion());
}

/*
* 오토인덱스 페이지를 만듭니다.
* @param 오토인덱스를 만들 경로
* @return 완성된 body
*/
std::string NormalResponse::makeAutoIndexPage(std::string resourcePath)
{
	std::string body;
	std::string addr = "http://" + request.getHeader()["Host"] + "/"; //하이퍼링크용 경로

	body += "<!DOCTYPE html>\n";
	body += "<html>\n";
	body += "<head>\n";
	body += "</head>\n";
	body += "<body>\n";
	body += "<h1> Index of "+ request.getUri() + "</h1>\n";

	DIR *dir = NULL;
	if ((dir = opendir(resourcePath.c_str())) == NULL)
		throwErrorResponse(500, request.getHttpVersion());

	struct dirent *file = NULL;
	while ((file = readdir(dir)) != NULL)
		body += "<a href=\"" + addr + file->d_name + "\">" + file->d_name + "</a><br>\n";

	closedir(dir);

	body += "</body>\n";
	body += "</html>\n";

	addContentLengthHeader(body.size());
	return (body);
}
