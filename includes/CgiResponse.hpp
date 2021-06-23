#ifndef CGIRESPONSE_HPP
#define CGIRESPONSE_HPP

#include "ResponseMaker.hpp"
#include "CgiWriter.hpp"
#include "Resource.hpp"

#define CGI_DIR		"./cgi_files/"
#define CGI_PATH	"cgi_result"

class CgiResponse : public ResponseMaker
{
	private:
		std::map<std::string, std::string> metaVariable;


		CgiResponse();

	public:
		CgiResponse(const Request&, const Server&, const Location&);
		CgiResponse(const CgiResponse &cg);
		CgiResponse &operator=(const CgiResponse &cg);
		~CgiResponse();
		
		void	makeVariable(int clientFd);
		char**	makeCgiEnvp();
		void	cgiResponse(int clientFd);

		static	Response	cgiResultPasring(std::string cgiResult);

};


#endif