# include "webserv.h"
# include "ResponseHandler.hpp"
# include "Server.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ResponseHandler::ResponseHandler(Request &Req, std::vector<Server>::iterator server)
:_Req(Req), _server(server){
	this->_mimeType[".aac"] = "audio/aac";
	this->_mimeType[".abw"] = "application/x-abiword";
	this->_mimeType[".arc"] = "application/octet-stream";
	this->_mimeType[".avi"] = "video/x-msvideo";
	this->_mimeType[".azw"] = "application/vnd.amazon.ebook";
	this->_mimeType[".bin"] = "application/octet-stream";
	this->_mimeType[".bz"] = "application/x-bzip";
	this->_mimeType[".bz2"] = "application/x-bzip2";
	this->_mimeType[".csh"] = "application/x-csh";
	this->_mimeType[".css"] = "text/css";
	this->_mimeType[".csv"] = "text/csv";
	this->_mimeType[".doc"] = "application/msword";
	this->_mimeType[".epub"] = "application/epub+zip";
	this->_mimeType[".gif"] = "image/gif";
	this->_mimeType[".htm"] = "text/html";
	this->_mimeType[".html"] = "text/html";
	this->_mimeType[".ico"] = "image/x-icon";
	this->_mimeType[".ics"] = "text/calendar";
	this->_mimeType[".jar"] = "Temporary Redirect";
	this->_mimeType[".jpeg"] = "image/jpeg";
	this->_mimeType[".jpg"] = "image/jpeg";
	this->_mimeType[".js"] = "application/js";
	this->_mimeType[".json"] = "application/json";
	this->_mimeType[".mid"] = "audio/midi";
	this->_mimeType[".midi"] = "audio/midi";
	this->_mimeType[".mpeg"] = "video/mpeg";
	this->_mimeType[".mpkg"] = "application/vnd.apple.installer+xml";
	this->_mimeType[".odp"] = "application/vnd.oasis.opendocument.presentation";
	this->_mimeType[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	this->_mimeType[".odt"] = "application/vnd.oasis.opendocument.text";
	this->_mimeType[".oga"] = "audio/ogg";
	this->_mimeType[".ogv"] = "video/ogg";
	this->_mimeType[".ogx"] = "application/ogg";
	this->_mimeType[".pdf"] = "application/pdf";
	this->_mimeType[".ppt"] = "application/vnd.ms-powerpoint";
	this->_mimeType[".rar"] = "application/x-rar-compressed";
	this->_mimeType[".rtf"] = "application/rtf";
	this->_mimeType[".sh"] = "application/x-sh";
	this->_mimeType[".svg"] = "image/svg+xml";
	this->_mimeType[".swf"] = "application/x-shockwave-flash";
	this->_mimeType[".tar"] = "application/x-tar";
	this->_mimeType[".tif"] = "image/tiff";
	this->_mimeType[".tiff"] = "image/tiff";
	this->_mimeType[".ttf"] = "application/x-font-ttf";
	this->_mimeType[".vsd"] = " application/vnd.visio";
	this->_mimeType[".wav"] = "audio/x-wav";
	this->_mimeType[".weba"] = "audio/webm";
	this->_mimeType[".webm"] = "video/webm";
	this->_mimeType[".webp"] = "image/webp";
	this->_mimeType[".woff"] = "application/x-font-woff";
	this->_mimeType[".xhtml"] = "application/xhtml+xml";
	this->_mimeType[".xls"] = "application/vnd.ms-excel";
	this->_mimeType[".xml"] = "application/xml";
	this->_mimeType[".xul"] = "application/vnd.mozilla.xul+xml";
	this->_mimeType[".zip"] = "application/zip";
	this->_mimeType[".3gp"] = "video/3gpp audio/3gpp";
	this->_mimeType[".3g2"] = "video/3gpp2 audio/3gpp2";
	this->_mimeType[".7z"] = "application/x-7z-compressed";
};

ResponseHandler::ResponseHandler(const ResponseHandler & src)
:_Req(src._Req), _mimeType(src._mimeType) {};

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
		this->_Req = rhs._Req;
		this->_responseHeader = rhs._responseHeader;
		this->_resourcePath = rhs._resourcePath;
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
int ResponseHandler::_checkPath(std::string path)
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
		_location = _server->getLocation(_Req.getUri());
		if (_location == NULL)
			_throwErrorResponse(NOT_FOUND, _Req.getHttpVersion());

		if (!_location->getOption("allow_method").empty() && _Req.getMethod() != "GET" && _Req.getMethod() != "HEAD")
		{
			std::string allow = _location->getOption("allow_method");
			if (allow.find(_Req.getMethod()) == std::string::npos)
				_throwErrorResponse(METHOD_NOT_ALLOWED, _Req.getHttpVersion());
		}

		if (_Req.getMethod() == "TRACE")
			_makeTraceResponse();
		else if (_Req.getMethod() == "OPTIONS")
			_makeOptionResponse();
		else if (_Req.getMethod() == "CONNECT")
			_makeConnectResponse();

		this->_resourcePath = _Req.getUri(); //나중에 root 들어오면 앞에 붙여주세요
		//경로 한번 더 검사-> 존재 안하면
		if (_checkPath(this->_resourcePath) == NOT_FOUND && _Req.getMethod() != "PUT" && _Req.getMethod() != "POST")
			_throwErrorResponse(NOT_FOUND, _Req.getHttpVersion());

		/*
		* 여기에는
		* 1. 메소드에 따라 CGI 호출 여부를 결정하는 부분
		* 2. Transfer-Encoding 처리
		* 3. redirection 처리
		* 가 추가되어야 합니다.
		*/

		if (_Req.getMethod() == "GET" || _Req.getMethod() == "POST")
			_makeGetResponse(0);
		if (_Req.getMethod() == "HEAD")
			_makeGetResponse(HEAD_METHOD);
		if (_Req.getMethod() == "PUT")
			_makePutResponse();
		if (_Req.getMethod() == "DELETE")
			_makeDeleteResponse();
	}
	catch (Response &e)
	{
		return e;
	}

	_responseHeader.clear();
	_resourcePath.clear();
	return Response(500, _responseHeader, _makeHTMLPage(ft_itoa(500)), _Req.getHttpVersion());
}

/*
* GET 메소드의 Response를 생성합니다.
* @param send_body HEAD를 실행했을 경우 send_body 가 false값으로 들어옵니다.
* 기본 헤더 : Date, Server
* 엔티티 헤더 : Last-Modified
* 컨텐츠 헤더 : Content-Language, Content-Length, Content-Location,	Content-Type
* 반환 Status-Code : 404(찾을 수 없음), 200(성공), 500(내부 서버 오류), 403(권한이 없음)
*/
void ResponseHandler::_makeGetResponse(int httpStatus)
{
	std::string body;
	int fd;
	struct stat	sb;
	int res;

	addDateHeader();
	addServerHeader();
	if (_checkPath(this->_resourcePath) == ISDIR)
	{
		if (this->_resourcePath[this->_resourcePath.length() - 1] != '/')
			this->_resourcePath += '/';

		if (!_location->getOption("index").empty())
		{
			bool indexFileFlag = false;
			char **indexFile = ft_split(_server->getOption("index").c_str(), ' ');
			for (int i = 0; indexFile[i]; i++)
			{
				struct stat buffer;
				if (stat((this->_resourcePath + indexFile[i]).c_str(), &buffer) == 0)
				{
					this->_resourcePath = this->_resourcePath + indexFile[i];
					indexFileFlag = true;
					break ;
				}
			}
			if (indexFileFlag == false && _location->getOption("autoindex") == "on")
			{
				addContentTypeHeader(".html");
				throw Response(200, this->_responseHeader, _Req.getMethod() != "HEAD" ? _makeAutoIndexPage(this->_resourcePath) : "", _Req.getHttpVersion());
			}
		}
	}
	if ((fd = open(this->_resourcePath.c_str(), O_RDONLY)) < 0)
		_throwErrorResponse(SERVER_ERR, _Req.getHttpVersion());
	if (fstat(fd, &sb) < 0)
	{
		close(fd);
		_throwErrorResponse(SERVER_ERR, _Req.getHttpVersion());
	}
	char *buffer;
	while((res = get_next_line(fd, &buffer)) > 0)
	{
		body += buffer;
		free(buffer);
	}
	if (res < 0)
		_throwErrorResponse(SERVER_ERR, _Req.getHttpVersion());
	get_next_line(fd, &buffer);
	body += buffer;
	free(buffer);

	addContentTypeHeader(_resourcePath);
	addContentLanguageHeader();
	addContentLocationHeader();
	addContentLengthHeader((int)sb.st_size);
	addLastModifiedHeader(_resourcePath);
	if (httpStatus == HEAD_METHOD)
		throw Response(200, _responseHeader, "", _Req.getHttpVersion());
	throw Response(200, _responseHeader, body, _Req.getHttpVersion());
}

/*
* 클라이언트가 서버에게 지정한 URL에 지정한 데이터를 저장할 것을 요청한다.
* RESPONSE
* 기본 헤더 : Date, Server
* 컨텐츠 헤더 :Content-Location
* 반환 Status-Code :
* 기존에 없는 것을 새로이 생성 - 201 (Created)
* 기존에 있는 것을 성공적으로 수정 - 200 (OK) 또는 204 (No Content) 응답
* 실패 : 500(SERVER ERR)
* 권한없음 : 403(forbidden)
*/
void ResponseHandler::_makePutResponse(void)
{
	int fd;
	int pathType = _checkPath(this->_resourcePath);

	switch (pathType)
	{
		case NOT_FOUND :
		{

			if ((fd = open(this->_resourcePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC)) < 0)
				_throwErrorResponse(SERVER_ERR, _Req.getHttpVersion());

			write(fd, _Req.getRawBody().c_str(), ft_atoi(_Req.getHeader()["Content-length"].c_str()));
			close(fd);
			addContentLocationHeader();
			throw Response(201, _responseHeader, "", _Req.getHttpVersion());
		}
		case ISFILE :
		{
			if ((fd = open(this->_resourcePath.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644)) < 0)
				_throwErrorResponse(SERVER_ERR, _Req.getHttpVersion());

			write(fd, _Req.getRawBody().c_str(), ft_atoi(_Req.getHeader()["Content-length"].c_str()));\
			close(fd);
			addContentLocationHeader();
			throw Response(204, _responseHeader, "", _Req.getHttpVersion());
		}
		default :
			_throwErrorResponse(FORBIDDEN, _Req.getHttpVersion());
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
void ResponseHandler::_makeDeleteResponse(void)
{
	addDateHeader();
	addServerHeader();
	if (_checkPath(this->_resourcePath) == ISFILE)
	{
		unlink(this->_resourcePath.c_str());
		if (!_location->getOption("allow_method").empty())
		{
			std::string allow = _location->getOption("allow_method");
			addAllowHeader(allow);
		}
		throw Response(200, _responseHeader, _makeHTMLPage("File deleted"), _Req.getHttpVersion());
	}
	_throwErrorResponse(NOT_FOUND, _Req.getHttpVersion());
}

/*
* 클라이언트가 서버에게 송신한 요청을 반환한다.
*/
void ResponseHandler::_makeTraceResponse()
{
	addContentTypeHeader("message/http");
	throw Response(200, _Req.getHeader(), "", _Req.getHttpVersion());
}

/*
* 해당 URL에서 지원하는 요청 method 목록을 요청한다.
*/
void ResponseHandler::_makeOptionResponse()
{
	if (!_location->getOption("allow_method").empty())
	{
		std::string allow = _location->getOption("allow_method");
		addAllowHeader(allow);
	}
	throw Response(200, _responseHeader, "", _Req.getHttpVersion());
}

/*
* 클라이언트가 특정 종류의 프록시 서버에게 연결을 요청한다.
*/
void ResponseHandler::_makeConnectResponse()
{
	addHostHeader();
	throw Response(200, _responseHeader, "", _Req.getHttpVersion());
}

/*
* 오토인덱스 페이지를 만듭니다.
* @param 오토인덱스를 만들 경로
* @return 완성된 body
*/
std::string ResponseHandler::_makeAutoIndexPage(std::string _resourcePath)
{
	std::string body;
	std::string addr = "http://" + _Req.getHeader()["Host"] + "/"; //하이퍼링크용 경로

	body += "<!DOCTYPE html>";
	body += "<html>";
	body += "<head>";
	body += "</head>";
	body += "<body>";
	body += "<h1> Index of "+ _Req.getUri() + "</h1>";

	DIR *dir = NULL;
	if ((dir = opendir(_resourcePath.c_str())) == NULL)
		_throwErrorResponse(500, _Req.getHttpVersion());

	struct dirent *file = NULL;
	while ((file = readdir(dir)) != NULL)
		body += "<a href=\"" + addr + file->d_name + "\">" + file->d_name + "</a><br>";

	closedir(dir);

	body += "</body>";
	body += "</html>";

	addContentLengthHeader(body.size());
	return (body);
}

/*
* HTML 페이지를 만듭니다.
* @param HTML 상에 출력할 string
* @return HTML 코드 string
*/
std::string ResponseHandler::_makeHTMLPage(std::string str)
{
	std::string body;

	addContentTypeHeader(".html");
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

void ResponseHandler::_throwErrorResponse(int httpStatus, std::string version) throw(Response)
{
	switch (httpStatus)
	{
		case NOT_FOUND:
			throw Response(404, _responseHeader, _makeHTMLPage(ft_itoa(404)), version);
		case SERVER_ERR:
			throw Response(500, _responseHeader, _makeHTMLPage(ft_itoa(500)), version);
		case FORBIDDEN:
			throw Response(403, _responseHeader, _makeHTMLPage(ft_itoa(403)), version);
		case METHOD_NOT_ALLOWED:
			throw Response(405, _responseHeader, _makeHTMLPage(ft_itoa(405)), version);
		default:
			throw Response(404, _responseHeader, _makeHTMLPage(ft_itoa(404)), version);
	}
}
