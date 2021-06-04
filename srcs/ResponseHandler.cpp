#include "webserv.h"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ResponseHandler::ResponseHandler(Request &Req)
:_Req(Req) {
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
	if (exist != 0)
		return (NOT_FOUND);
	if (S_ISREG(buffer.st_mode))
		return (ISFILE);
	else if (S_ISDIR(buffer.st_mode))
		return (ISDIR);
}

/*
* 입력 메소드와 헤더를 판단해 대응하는 응답값을 출력합니다.
*/
void ResponseHandler::makeResponse()
{
	/*
		여기에는...
		특정한 헤더가 들어오면, 컨텐츠 협상을 통해서 언어를 정하거나(Accept-Charset, Accept-Language),
		클라이언트의 소프트웨어 정보를 보내주거나(User-Agent) 하는 부분이 추가되어야 합니다.
		또한 Host 헤더가 들어오면, Host헤더의 value 포트로 이동하게 해주세요!
	*/
	try{
		this->_resourcePath = 루트변수 + _Req.getUri(); //나중에 root 들어오면 앞에 붙여주세요

		//디렉토리면
		if (_checkPath(this->_resourcePath) == ISDIR)
		{
			if (this->_resourcePath[this->_resourcePath.length() - 1] != '/')
                this->_resourcePath += '/';
			if (오토인덱스변수)
				throw Response(200, this->_responseHeader, _Req.getMethod() != "HEAD" ? _makeAutoIndexPage(this->_resourcePath) : "", _Req.getHttpVersion());
				//_makeAutoIndexPage 함수 제작중
		}
		//경로 한번 더 검사-> 존재 안하면
		if (_checkPath(this->_resourcePath) == NOT_FOUND && _Req.getMethod() != "PUT" && _Req.getMethod() != "POST")
				_throwErrorResponse(404, _Req.getHttpVersion());

		/*
		* 여기에는 메소드에 따라 CGI 호출 여부를 결정하는 부분이 추가되어야 합니다.
		*/

		if (_Req.getMethod() == "GET" || _Req.getMethod() == "POST")
			_makeGetResponse(0);
		if (_Req.getMethod() == "HEAD")
			_makeGetResponse(HEAD_METHOD);

	}
	catch (Response &e)
	{
		e.getMessage();
		return ;
	}

	_responseHeader.clear();
	_resourcePath.clear();
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

	//open 후 read-> 구조체에 파일 객체 담기. 그 후 body에 객체를 담기
	if ((fd = open(this->_resourcePath.c_str(), O_RDONLY)) < 0)
		_throwErrorResponse(500, _Req.getHttpVersion());
	if (fstat(fd, &sb) < 0)
	{
		close(fd);
		_throwErrorResponse(500, _Req.getHttpVersion());
	}
	char *buffer;
	while((res = get_next_line(fd, &buffer)) > 0)
	{
		body += buffer;
		free(buffer);
	}
	if (res < 0)
	{
		free(buffer);
		_throwErrorResponse(500, _Req.getHttpVersion());
	}
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

void ResponseHandler::_throwErrorResponse(int httpStatus, std::string version) throw()
{
	switch (httpStatus)
	{
		case NOT_FOUND:
			throw Response(404, _responseHeader, _makeErrorPage(404), version);
		case SERVER_ERR:
			throw Response(500, _responseHeader, _makeErrorPage(500), version);
		case FORBIDDEN:
			throw Response(403, _responseHeader, _makeErrorPage(403), version);
		default:
			throw Response(404, _responseHeader, _makeErrorPage(404), version);
	}
}

/*
* 오토인덱스 페이지를 만듭니다.
* 미작성
*/
std::string ResponseHandler::_makeAutoIndexPage(std::string _resourcePath)
{
}

/*
* 상태 코드에 따라 에러 페이지를 만듭니다.
* @param 상태 코드
* @return HTML 코드 string
*/
std::string ResponseHandler::_makeErrorPage(int statusCode)
{
	std::string body;

	addContentTypeHeader(".html");
	body += "<!DOCTYPE html>\n";
	body += "<html>\n";
	body += "<head>\n";
	body += "</head>\n";
	body += "<body>\n";
	body += "<h1>";
	body += ft_itoa(statusCode);
	body += "<h1>\n";
	body += "</body>\n";
	body += "</html>";
	return (body);
}