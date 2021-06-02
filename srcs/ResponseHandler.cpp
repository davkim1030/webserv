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
* 입력 메소드와 헤더를 판단해 대응하는 응답값을 출력합니다.
*/
void ResponseHandler::makeResponse()
{

	/*
		여기에는...
		메소드에 따라 CGI 호출 여부를 결정하는 부분도 추가되어야 합니다.
		또한 입력으로 특정한 헤더가 들어오면, 컨텐츠 협상을 통해서 언어를 정하거나(Accept-Charset, Accept-Language),
		클라이언트의 소프트웨어 정보를 보내주거나(User-Agent) 하는 부분이 추가되어야 합니다.

		또한 Host 헤더가 들어오면, Host헤더의 value 포트로 이동하게 해주세요!
	*/

	if (_Req.getMethod() == "GET" || _Req.getMethod() == "POST")
		_makeGetResponse();

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
void ResponseHandler::_makeGetResponse(bool send_body)
{
	int statusCode;
	std::string body;
	std::string version = _Req.getHttpVersion();

	addDateHeader();
	addServerHeader();
	try{

		//uri가 유효하면 open 후 구조체에 파일 객체 담기. 그 후 body에 객체를 담기
		//파일을 어떻게 body에 담는지 공부필요

		content_bytes = readBinaryFile(ressource_path);
		ressource_content = reinterpret_cast<unsigned char *>(&content_bytes[0]);
		// headers["Content-Type"] = _getMIMEType(ressource_path);
		pathType(ressource_path, &file_date);
		// headers["Last-Modified"] = _formatTimestamp(file_date);
		addContentTypeHeader();
		addContentLanguageHeader();
		addContentLocationHeader();
		addContentLengthHeader();
		addLastModifiedHeader();
		if (파일 없음)
			throw Response(404, _responseHeader, makeErrorPage(404), version);
		if (서버 오류)
			throw Response(500, _responseHeader, makeErrorPage(500), version);
		if (권한 없음)
			throw Response(403, _responseHeader, makeErrorPage(403), version);
		if (send_body)
			throw Response(200, _responseHeader, "", version);
		throw Response(200, _responseHeader, body, version);
	}
	catch (Response &e)
	{
		e.getMessage();
		return ;
	}
}




//리다이렉트 기본헤더 : Location
