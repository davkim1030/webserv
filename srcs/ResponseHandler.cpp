# include "webserv.h"
# include "ResponseHandler.hpp"
# include "Server.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ResponseHandler::ResponseHandler(const Request &request, const Server &server)
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
	cgi 처리 단계
	1. 들어온 파일 확인해서 cgi 확장자인지 확인
	2. cgi 이름에 맞는 파일이 있는지 찾기 ex) test.bla 들어오면 cgi_test 찾기
	3. uri 내부에 ?가 있는지 확인 후 ? 뒤에 있는 정보들은 query_string으로 저장
	4. 기본 환경변수 목록 만들기 -> map으로 저장해두면 편할 것 같음
	5. 저장된 환경변수 char** 형태로 가공
	6. 완성된 환경변수를 이용 해 cgi call
		6-1. pipe 생성 후 fcntl로 파일 속성 변경 (NON_BLOCK)
		6-2. fork로 프로세스 분리
		6-3. 가공된 정보를 통해 cgi 실행
		6-4. execve(실행할 파일 이름, (여긴 좀 더 알아볼 것_), 가공해둔 envp)
	7. execve에서 실행된 결과를 pipe를 통해 가져와서 body로 넘겨주기


*/
char** ResponseHandler::makeCgiEnvp()
{
	if (metaVariable["PATH_INFO"].empty())
		metaVariable["PATH_INFO"] = request.getUri();

	metaVariable["AUTH_TYPE"] = request.getHeader()["Authorization"];
	if (metaVariable["AUTH_TYPE"].empty())
		metaVariable["AUTH_TYPE"] = "null";

	metaVariable["CONTENT_LENGTH"] = request.getHeader()["Content-Length"];
	if (metaVariable["CONTENT_LENGTH"].empty())
		metaVariable["CONTENT_LENGTH"] = "0";

	metaVariable["CONTENT_TYPE"] = request.getHeader()["Content-Type"];
	if (metaVariable["CONTENT_TYPE"].empty())
		metaVariable["CONTENT_TYPE"] = "null";
	metaVariable["GATEWAY_INTERFACE"] = "Cgi/1.1";

	metaVariable["REQUEST_URI"] = request.getUri();
	if (metaVariable.count("PATH_INFO") == 1)
		metaVariable["REQUEST_URI"] = metaVariable["PATH_INFO"];

	// /test/cgi.bla/path/?query_string -> 이렇게 들어올 수 있음
	// 뒤에 아무것도 안들어오는 경우, /만 들어오고 끝나는 경우, /path 하고 들어오는 경우
	//   uri 통째로 써주기 ,      /만 써주기,             /path 써주기

	metaVariable["PATH_TRANSLATED"] = location.getOption("root") + metaVariable["PATH_INFO"].substr(1);
	metaVariable["REMOTE_ADDR"] = "127.0.0.1"; // 클라이언트의 IP
	// metaVariable["REMOTE_IDENT"] = ""; // 없어두됨
	// metaVariable["REMOTE_USER"] = ""; // 없어두됨 REMOTE 둘 다 AUTH 파일에 넘겨져 온다고 생각 중

	metaVariable["REQUEST_METHOD"] = "GET";
	metaVariable["SERVER_NAME"] = "my server name";
	metaVariable["SERVER_PORT"] = "server port";
	metaVariable["SERVER_PROTOCOL"] = "HTTP/1.1";
	metaVariable["SERVER_SOFTWARE"] = "joockim/1.1";


	char **res = (char **)malloc(sizeof(char *) * metaVariable.size() + 1);
	int i = 0;
	for (std::map<std::string, std::string>::const_iterator it = metaVariable.begin();
	it != metaVariable.end(); it++)
	{
		std::string temp = std::string(it->first + "=" + it->second);
		std::cout << temp << std::endl;
		res[i] = ft_strdup(temp.c_str());
		i++;
	}
	res[i] = NULL;
	return res;
}

void ResponseHandler::cgiResponse()
{
	pid_t pid;

	int fd[2];
	if (pipe(fd) == -1)
		throwErrorResponse(500, request.getHttpVersion());
	int fd_read = fd[0];
	int fd_write = fd[1];

	fcntl(fd[1], F_SETFL, O_NONBLOCK);

	std::string tempFile = "test_file";
	int fd_temp = open(tempFile.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0666);
	if (fd_temp == -1)
		throwErrorResponse(500, request.getHttpVersion());

	if ((pid = fork()) == -1)
		throwErrorResponse(500, request.getHttpVersion());
	else if (pid == 0)
	{
		std::cout << "child " << std::endl;
		close(fd_write);
		dup2(fd_read, 0);
		dup2(fd_temp, 1);
		char *argv[3];
		argv[0] = strdup(location.getOption("cgi_path").c_str());
		argv[1] = strdup((location.getOption("root") + metaVariable["SCRIPT_NAME"]).c_str());
		argv[2] = NULL;
		execve(argv[0], argv, makeCgiEnvp());
		exit(1);
	}
}

// cgi 실행 여부 판단
bool ResponseHandler::isCgi()
{
	std::string uri = request.getUri().substr(location.getPath().length());
	std::vector<std::string> ext = location.getCgiExtensionVector();

	for (std::vector<std::string>::iterator it = ext.begin(); it != ext.end(); it++)
	{
		int index = uri.find(*it);
		if (index != std::string::npos && uri.compare(index, it->length(), *it) == 0)
		{
			int queryIndex;
			if ((queryIndex = uri.find('?')) != -1)
			{
				metaVariable["QUERY_STRING"] = uri.substr(queryIndex + 1);
				uri = uri.substr(0, queryIndex);
			}
			metaVariable["SCRIPT_NAME"] = uri.substr(0, index + it->length());
			uri = uri.substr(index);
			int pathIndex = uri.find('/');
			if (pathIndex != std::string::npos)
			{
				metaVariable["PATH_INFO"] = uri.substr(pathIndex);
				uri = uri.substr(0, pathIndex);
			}
			if(uri.compare(0, it->length() + 1, *it) != 0)
				return false;

			return true;
		}
	}

	return (false);
}



Location ResponseHandler::findLocation(std::string uri)
{
	std::vector<Location> ser = server.getLocationVector();
	Location res;
	for (std::vector<Location>::iterator it = ser.begin(); it != ser.end(); it++)
	{
		std::string path = it->getPath();
		std::string tempPath = path + '/';
		if (uri.compare(0, path.length(), path) == 0 || uri.compare(0, tempPath.length(), tempPath) == 0)
			res = *it;
	}
	return res;
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
		location = findLocation(request.getUri());
		if (location.getPath().empty())
			throwErrorResponse(NOT_FOUND, request.getHttpVersion());

		if (isCgi())
		{
			std::cout << "cgi on " << std::endl;
			cgiResponse();
			throwErrorResponse(500, request.getHttpVersion());
		}
		std::cout << "cgi off" << std::endl;

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
	return Response(500, responseHeader, makeHTMLPage("500"), request.getHttpVersion());
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
	char	*strTmp = ft_itoa(httpStatus);
	std::string body = makeHTMLPage(strTmp);
	free(strTmp);
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
