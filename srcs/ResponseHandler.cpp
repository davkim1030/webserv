# include "webserv.h"
# include "ResponseHandler.hpp"
# include "Server.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ResponseHandler::ResponseHandler(Request &request, Server &server)
:request(request), server(server){
	this->mimeType[".aac"] = "audio/aac";
	this->mimeType[".abw"] = "application/x-abiword";
	this->mimeType[".arc"] = "application/octet-stream";
	this->mimeType[".avi"] = "video/x-msvideo";
	this->mimeType[".azw"] = "application/vnd.amazon.ebook";
	this->mimeType[".bin"] = "application/octet-stream";
	this->mimeType[".bz"] = "application/x-bzip";
	this->mimeType[".bz2"] = "application/x-bzip2";
	this->mimeType[".csh"] = "application/x-csh";
	this->mimeType[".css"] = "text/css";
	this->mimeType[".csv"] = "text/csv";
	this->mimeType[".doc"] = "application/msword";
	this->mimeType[".epub"] = "application/epub+zip";
	this->mimeType[".gif"] = "image/gif";
	this->mimeType[".htm"] = "text/html";
	this->mimeType[".html"] = "text/html";
	this->mimeType[".ico"] = "image/x-icon";
	this->mimeType[".ics"] = "text/calendar";
	this->mimeType[".jar"] = "Temporary Redirect";
	this->mimeType[".jpeg"] = "image/jpeg";
	this->mimeType[".jpg"] = "image/jpeg";
	this->mimeType[".js"] = "application/js";
	this->mimeType[".json"] = "application/json";
	this->mimeType[".mid"] = "audio/midi";
	this->mimeType[".midi"] = "audio/midi";
	this->mimeType[".mpeg"] = "video/mpeg";
	this->mimeType[".mpkg"] = "application/vnd.apple.installer+xml";
	this->mimeType[".odp"] = "application/vnd.oasis.opendocument.presentation";
	this->mimeType[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	this->mimeType[".odt"] = "application/vnd.oasis.opendocument.text";
	this->mimeType[".oga"] = "audio/ogg";
	this->mimeType[".ogv"] = "video/ogg";
	this->mimeType[".ogx"] = "application/ogg";
	this->mimeType[".pdf"] = "application/pdf";
	this->mimeType[".ppt"] = "application/vnd.ms-powerpoint";
	this->mimeType[".rar"] = "application/x-rar-compressed";
	this->mimeType[".rtf"] = "application/rtf";
	this->mimeType[".sh"] = "application/x-sh";
	this->mimeType[".svg"] = "image/svg+xml";
	this->mimeType[".swf"] = "application/x-shockwave-flash";
	this->mimeType[".tar"] = "application/x-tar";
	this->mimeType[".tif"] = "image/tiff";
	this->mimeType[".tiff"] = "image/tiff";
	this->mimeType[".ttf"] = "application/x-font-ttf";
	this->mimeType[".vsd"] = " application/vnd.visio";
	this->mimeType[".wav"] = "audio/x-wav";
	this->mimeType[".weba"] = "audio/webm";
	this->mimeType[".webm"] = "video/webm";
	this->mimeType[".webp"] = "image/webp";
	this->mimeType[".woff"] = "application/x-font-woff";
	this->mimeType[".xhtml"] = "application/xhtml+xml";
	this->mimeType[".xls"] = "application/vnd.ms-excel";
	this->mimeType[".xml"] = "application/xml";
	this->mimeType[".xul"] = "application/vnd.mozilla.xul+xml";
	this->mimeType[".zip"] = "application/zip";
	this->mimeType[".3gp"] = "video/3gpp audio/3gpp";
	this->mimeType[".3g2"] = "video/3gpp2 audio/3gpp2";
	this->mimeType[".7z"] = "application/x-7z-compressed";
};

ResponseHandler::ResponseHandler(const ResponseHandler & src)
: request(src.request), server(src.server), location(src.location), resourcePath(src.resourcePath), responseHeader(src.responseHeader), mimeType(src.mimeType) {};

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

ResponseHandler::~ResponseHandler() {};

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

ResponseHandler & ResponseHandler::operator=( ResponseHandler const & rhs )
{
	if ( this != &rhs )
	{
		this->request = rhs.request;
		this->responseHeader = rhs.responseHeader;
		this->resourcePath = rhs.resourcePath;
	}
	return *this;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

/*
 * 경로가 존재하는지 / 파일인지 / 경로인지 판별합니다.
 * @param 체크할 경로
 * @return 경로가 존재하지 않을 경우 0 반환, 파일일 경우 1 반환, 디렉토리일경우 2 반환
 */
int ResponseHandler::checkPath(std::string path)
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

/*
* 입력 메소드와 헤더를 판단해 대응하는 응답값을 출력합니다.
* @return 응답값을 담은 Response
*/
Response ResponseHandler::makeResponse()
{
	/*
		여기에는...
		특정한 헤더가 들어오면, 컨텐츠 협상을 통해서 언어를 정하거나(Accept-Charset, Accept-Language),
		클라이언트의 소프트웨어 정보를 보내주거나(User-Agent) 하는 부분이 추가되어야 합니다.
		또한 Host 헤더가 들어오면, Host헤더의 value 포트로 이동하게 해주세요!
	*/

	try{
		/*CGI를 넣어주세요.*/
		//location에서 uri를 찾지말고 uri에서 location을 찾아야합니다.
		location = server.getLocation(request.getUri());

		if (location == NULL)
			throwErrorResponse(NOT_FOUND, request.getHttpVersion());
		if (!location->getOption("allow_method").empty() && request.getMethod() != "GET" && request.getMethod() != "HEAD")
		{
			std::string allow = location->getOption("allow_method");
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

		// 로케이션 이름(가상 디렉토리)들 중에서 request의 uri와 같은 것이 있는지 찾기
		std::vector<Location> loc = server.getLocationVector();
		for (std::vector<Location>::iterator iter = loc.begin();
				iter != loc.end(); iter++)
		{
			// request uri랑 같은 가상 디렉토리가 있음
			if (iter->getPath() == request.getUri())
			{
				location = &(*iter);
				// 있으면 거기서 인덱스 찾기
				if (iter->getOption("index") != "")
				{
					resourcePath = iter->getOption("root");
					break ;
				}
			}
		}
<<<<<<< HEAD
		// 못 찾으면 기본 값 주기
=======
>>>>>>> 9027f4c9e83bb8747c1996c6899c6afc84414268
		if (resourcePath == "")
			this->resourcePath = location->getOption("root") + request.getUri(); //root 들어오면 더해주세요

		if (checkPath(this->resourcePath) == NOT_FOUND && request.getMethod() != "PUT" && request.getMethod() != "POST")
			throwErrorResponse(NOT_FOUND, request.getHttpVersion());
		if (request.getMethod() == "GET")
			makeGetResponse(0);
		if (request.getMethod() == "HEAD")
			makeHeadResponse();
		if (request.getMethod() == "POST")
			makePostResponse();
		if (request.getMethod() == "PUT")
			makePutResponse();
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
	return Response(500, responseHeader, makeHTMLPage(ft_itoa(500)), request.getHttpVersion());
}

/*
* GET 메소드의 Response를 생성합니다.
* 기본 헤더 : Date, Server
* 엔티티 헤더 : Last-Modified
* 컨텐츠 헤더 : Content-Language, Content-Length, Content-Location,	Content-Type
* 반환 Status-Code : 404(찾을 수 없음), 200(성공), 500(내부 서버 오류), 403(권한이 없음)
*/
void ResponseHandler::makeGetResponse(int httpStatus)
{
	std::string body;
	int fd;
	struct stat	sb;
	int res;

	addDateHeader();
	addServerHeader();
	if (checkPath(this->resourcePath) == ISDIR)
	{

		if (this->resourcePath[this->resourcePath.length() - 1] != '/')
			this->resourcePath += '/';

		if (!location->getOption("index").empty())
		{
			bool indexFileFlag = false;
			char **indexFile = ft_split(location->getOption("index").c_str(), ' ');
			for (int i = 0; indexFile[i]; i++)
			{
				struct stat buffer;
				if (stat((this->resourcePath + indexFile[i]).c_str(), &buffer) == 0)
				{
					this->resourcePath = this->resourcePath + indexFile[i];
					indexFileFlag = true;
					break ;
				}
			}
			if (indexFileFlag == false && location->getOption("autoindex") == "on")
			{
				addContentTypeHeader(".html");
				throw Response(200, this->responseHeader, request.getMethod() != "HEAD" ? makeAutoIndexPage(this->resourcePath) : "", request.getHttpVersion());
			}
		}
	}
	if ((fd = open(this->resourcePath.c_str(), O_RDONLY)) < 0)
		throwErrorResponse(SERVER_ERR, request.getHttpVersion());
	if (fstat(fd, &sb) < 0)
	{
		close(fd);
		throwErrorResponse(SERVER_ERR, request.getHttpVersion());
	}
	char *buffer;
	while((res = get_next_line(fd, &buffer)) > 0)
	{
		body += buffer;
		body += "\n";
		free(buffer);
	}
	if (res < 0)
		throwErrorResponse(SERVER_ERR, request.getHttpVersion());
	body += buffer;
	free(buffer);
	addContentTypeHeader(fileExtension(resourcePath.substr(1)));
	addContentLanguageHeader();
	addContentLocationHeader();
	addContentLengthHeader((int)sb.st_size);
	addLastModifiedHeader(resourcePath);
	if (httpStatus == HEAD_METHOD)
		throw Response(200, responseHeader, "", request.getHttpVersion());
	throw Response(200, responseHeader, body, request.getHttpVersion());
}

/*
* HEAD 메소드의 Response를 생성합니다.
* allowed method인지 확인하고 이상이 없으면 makeGetResponse 함수를 호출한다.
*/
void ResponseHandler::makeHeadResponse(void)
{
	if (server.getOption("allowed_method").find("HEAD") == std::string::npos)
		throw Response(405, responseHeader, "", request.getHttpVersion());
	makeGetResponse(HEAD_METHOD);
}

/*
* 입력받은 파일의 확장자를 반환합니다.
*/
std::string ResponseHandler::fileExtension(std::string resourcePath)
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
*/

void ResponseHandler::makePostResponse(void)
{
	int fd;
	int pathType = checkPath(this->resourcePath);

	switch (pathType)
	{
		case NOT_FOUND :
		{
			if ((fd = open(this->resourcePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0)
				throwErrorResponse(SERVER_ERR, request.getHttpVersion());

			write(fd, request.getRawBody().c_str(), ft_atoi(request.getHeader()["Content-Length"].c_str()));
			close(fd);
			addContentLocationHeader();
			throw Response(201, responseHeader, "", request.getHttpVersion());
		}
		case ISFILE :
		{
			if ((fd = open(this->resourcePath.c_str(), O_WRONLY | O_APPEND )) < 0)
				throwErrorResponse(SERVER_ERR, request.getHttpVersion());
			write(fd, request.getRawBody().c_str(), ft_atoi(request.getHeader()["Content-Length"].c_str()));
			close(fd);
			addContentLocationHeader();
			throw Response(200, responseHeader, "", request.getHttpVersion());
		}
		default :
			throwErrorResponse(FORBIDDEN, request.getHttpVersion());
	}
}

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
*/
void ResponseHandler::makePutResponse(void)
{
	int fd;
	int pathType = checkPath(this->resourcePath);

	switch (pathType)
	{
		case NOT_FOUND :
		{
			if ((fd = open(this->resourcePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0)
				throwErrorResponse(SERVER_ERR, request.getHttpVersion());

			write(fd, request.getRawBody().c_str(), ft_atoi(request.getHeader()["Content-Length"].c_str()));
			close(fd);
			addContentLocationHeader();
			throw Response(201, responseHeader, "", request.getHttpVersion());
		}
		case ISFILE :
		{
			if ((fd = open(this->resourcePath.c_str(), O_WRONLY | O_TRUNC)) < 0)
				throwErrorResponse(SERVER_ERR, request.getHttpVersion());
			write(fd, request.getRawBody().c_str(), ft_atoi(request.getHeader()["Content-Length"].c_str()));
			close(fd);
			addContentLocationHeader();
			throw Response(200, responseHeader, "", request.getHttpVersion());
		}
		default :
			throwErrorResponse(FORBIDDEN, request.getHttpVersion());
	}
}

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
void ResponseHandler::makeDeleteResponse(void)
{
	addDateHeader();
	addServerHeader();
	if (checkPath(this->resourcePath) == ISFILE)
	{
		unlink(this->resourcePath.c_str());
		if (!location->getOption("allow_method").empty())
		{
			std::string allow = location->getOption("allow_method");
			addAllowHeader(allow);
		}
		throw Response(200, responseHeader, makeHTMLPage("File deleted"), request.getHttpVersion());
	}
	throwErrorResponse(NOT_FOUND, request.getHttpVersion());
}

/*
* 클라이언트가 서버에게 송신한 요청을 반환한다.
*/
void ResponseHandler::makeTraceResponse()
{
	addContentTypeHeader("message/http");
	throw Response(200, request.getHeader(), "", request.getHttpVersion());
}

/*
* 해당 URL에서 지원하는 요청 method 목록을 요청한다.
*/
void ResponseHandler::makeOptionResponse()
{
	if (!location->getOption("allow_method").empty())
	{
		std::string allow = location->getOption("allow_method");
		addAllowHeader(allow);
	}
	throw Response(200, responseHeader, "", request.getHttpVersion());
}

/*
* 클라이언트가 특정 종류의 프록시 서버에게 연결을 요청한다.
*/
void ResponseHandler::makeConnectResponse()
{
	addHostHeader();
	throw Response(200, responseHeader, "", request.getHttpVersion());
}

/*
* 오토인덱스 페이지를 만듭니다.
* @param 오토인덱스를 만들 경로
* @return 완성된 body
*/
std::string ResponseHandler::makeAutoIndexPage(std::string resourcePath)
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

/*
* HTML 페이지를 만듭니다.
* @param HTML 상에 출력할 string
* @return HTML 코드 string
*/
std::string ResponseHandler::makeHTMLPage(std::string str)
{
	std::string body;

	body += "<!DOCTYPE html>\n";
	body += "<html>\n";
	body += "<head>\n";
	body += "</head>\n";
	body += "<body>\n";
	body += "<h1>";
	body += str;
	body += "<h1>\n";
	body += "</body>\n";
	body += "</html>";
	return (body);
}

void ResponseHandler::throwErrorResponse(int httpStatus, std::string version) throw(Response)
{
	std::string body = makeHTMLPage(ft_itoa(httpStatus));
	addDateHeader();
	addServerHeader();
	addContentTypeHeader(".html");
	addContentLengthHeader((int)body.length());
	switch (httpStatus)
	{
		case NOT_FOUND:
			throw Response(404, responseHeader, body, version);
		case SERVER_ERR:
			throw Response(500, responseHeader, body, version);
		case FORBIDDEN:
			throw Response(403, responseHeader, body, version);
		case METHOD_NOT_ALLOWED:
			throw Response(405, responseHeader, body, version);
		default:
			throw Response(404, responseHeader, body, version);
	}
}
