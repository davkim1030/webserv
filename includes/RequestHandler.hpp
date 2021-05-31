#ifndef REQUESTHANDLER_HPP
# define REQUESTHANDLER_HPP

# include "webserv.h"
class RequestHandler
{
	public:
	RequestHandler();
	RequestHandler( std::string const );
	RequestHandler( RequestHandler const & src );
	~RequestHandler();
	void RequestHandler::calcStatCode( Request Req );

	private:

};

#endif