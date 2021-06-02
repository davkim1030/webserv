#include "webserv.h"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ResponseHandler::ResponseHandler(Request &Req)
:_Req(Req) {};

ResponseHandler::ResponseHandler(const ResponseHandler & src)
:_Req(src._Req) {};

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

ResponseHandler::~ResponseHandler() { };

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
 * Check if a path is non-existent, a file, or a directory, and get its last-modified date
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
		메소드에 따라 CGI 호출 여부를 결정하는 부분이 추가되어야 합니다.
		또한 입력으로 특정한 헤더가 들어오면, 컨텐츠 협상을 통해서 언어를 정하거나(Accept-Charset, Accept-Language),
		클라이언트의 소프트웨어 정보를 보내주거나(User-Agent) 하는 부분이 추가되어야 합니다.

		또한 Host 헤더가 들어오면, Host헤더의 value 포트로 이동하게 해주세요!
	*/
	try{
		_resourcePath = _Req.getUri(); //나중에 root 들어오면 앞에 붙여주세요

		//디렉토리면
		if (_checkPath(_resourcePath) == ISDIR)
		{
			//URI 마지막에 '/' 없으면 '/'넣어주고 이것저것 해주셔야함

			if ()//가진 html중에 이름이 일치하는 페이지 있으면 그걸 보여주기

			else if (오토인덱스면)
				//오토인덱스 보내주고 여기서 끝내버림
				throw Response(200, _responseHeader, _Req.getMethod() != "HEAD" ? _makeAutoIndexPage(_resourcePath) : "", _Req.getHttpVersion());
				//_makeAutoIndexPage 함수 만들기

		}
		//경로 한번 더 검사-> 존재 안하면
		if (_checkPath(_resourcePath) == NOT_FOUND && _Req.getMethod != "PUT" && _Req.getMethod != "POST")
				throw Response(404, _responseHeader, _makeErrorPage(404), _Req.getHttpVersion());

			//CGI 검사

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
	std::string version = _Req.getHttpVersion();


	addDateHeader();
	addServerHeader();


		//uri가 유효하면 open 후 구조체에 파일 객체 담기. 그 후 body에 객체를 담기
		//파일을 어떻게 body에 담는지 공부필요
		if (fd = open(file.c_str(), O_RDONLY) < 0)
			throw Response(500, _responseHeader, _makeErrorPage(500), version);
		//contentlength만큼 파일에 우겨넣기 필요


		addContentTypeHeader();
		addContentLanguageHeader();
		addContentLocationHeader();
		addContentLengthHeader();
		addLastModifiedHeader();

	switch (httpStatus)
	{
		case NOT_FOUND:
			throw Response(404, _responseHeader, _makeErrorPage(404), version);
		case SERVER_ERR:
			throw Response(500, _responseHeader, _makeErrorPage(500), version);
		case FORBIDDEN:
			throw Response(403, _responseHeader, _makeErrorPage(403), version);
		case HEAD_METHOD:
			throw Response(200, _responseHeader, "", version);
		default:
			throw Response(200, _responseHeader, body, version);
	}
}


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
	return body ;
}

//리다이렉트 기본헤더 : Location
