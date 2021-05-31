#include "webserv.h"

RequestHandler::RequestHandler()
{
	Request Req();
};

RequestHandler::RequestHandler(std::string const buffer)
{
	Request Req(buffer);
	calcStatCode (Req);
};

RequestHandler::RequestHandler( RequestHandler const & src )
{
};

RequestHandler::~RequestHandler()
{

};

void RequestHandler::calcStatCode( Request Req )
{
	Req.getUri();
	Req.getRawBody();

	int _statusCode;
	char * _body;

	// ~~바디 읽어서 연산, 스테이터스코드 계산, cgi 호출 여부 정하기~~ //

		// GET
		// HEAD
		// POST
		// PATCH
		// PUT
		// DELETE
		// TRACE
		// CONNECT
		// OPTIONS

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

	Response Res(_statusCode, Req.getHeader(), _body, Req.getHttpVersion());
	std::cout << Res.getMessage();
}