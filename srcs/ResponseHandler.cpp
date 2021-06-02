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
		//this->_statusCode = rhs.getStatusCode();
		//this->_body = rhs.getBody();
		//this->_version = rhs.getVersion();
	}
	return *this;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

//Request ResponseHandler::getRequest(void) const { return this->_Req; }
//int ResponseHandler::getStatusCode(void) const { return this->_statusCode; }
//std::string ResponseHandler::getBody(void) const { return this->_body; }
//std::string ResponseHandler::getVersion(void) const { return this->_version; }
//std::map<std::string, std::string> ResponseHandler::getResponseHeader(void) const { return this->_responseHeader; }

//void ResponseHandler::setResponseHeader(std::map<std::string, std::string> header) { this->_responseHeader = header; };
//void ResponseHandler::setRequest(Request Req) { this->_Req = Req; };
//void ResponseHandler::setStatusCode(int code) { this->_statusCode = code; };
//void ResponseHandler::setBody(std::string body) { this->_body = body; };
//void ResponseHandler::setVersion(std::string version) { this->_version = version; };


/*

 * Check if a path is non-existent, a file, or a directory, and get its last-modified date
 * @param path the path to check
 * @param file_date a time_t value that will be set to the date of modification of the file
 * @return 0 if the path is non-existant, 1 if the path is a file, 2 if the path is a directory
 */
int checkPath(std::string path)
{
	struct stat buffer;

	int exist = stat(path.c_str(), &buffer);
	if (exist == 0)
	{
		if (S_ISREG(buffer.st_mode))
			return (1);
		else
			return (2);
	}
	else
		return (0);
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
	checkPath(_Req.getUri());

	if (_Req.getMethod() == "GET" || _Req.getMethod() == "POST")
		_makeGetResponse(0);
	if (_Req.getMethod() == "HEAD")
		_makeGetResponse(HEAD_METHOD);

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
	try{

		//uri가 유효하면 open 후 구조체에 파일 객체 담기. 그 후 body에 객체를 담기
		//파일을 어떻게 body에 담는지 공부필요

		open();

		addContentTypeHeader();
		addContentLanguageHeader();
		addContentLocationHeader();
		addContentLengthHeader();
		addLastModifiedHeader();

		switch (httpStatus)
		{
			case NOT_FOUND:
				throw Response(404, _responseHeader, makeErrorPage(404), version);
			case SERVER_ERR:
				throw Response(500, _responseHeader, makeErrorPage(500), version);
			case FORBIDDEN:
				throw Response(403, _responseHeader, makeErrorPage(403), version);
			case HEAD_METHOD:
				throw Response(200, _responseHeader, "", version);
			default:
				throw Response(200, _responseHeader, body, version);
		}
	}
	catch (Response &e)
	{
		e.getMessage();
		return ;
	}
}


std::string ResponseHandler::makeErrorPage(int statusCode)
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
