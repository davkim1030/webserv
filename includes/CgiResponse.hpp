#ifndef CGIRESPONSE_HPP
# define CGIRESPONSE_HPP

# include "ResponseMaker.hpp"
# include "CgiWriter.hpp"
# include "Resource.hpp"

#define CGI_DIR		"./cgi_files/"
#define CGI_PATH	"cgi_result"

/*
 * cgi를 통해 만들 response를 생성, 처리하는 클래스
 */
class CgiResponse : public ResponseMaker
{
	private:
		std::map<std::string, std::string> metaVariable;	// cgi의 메타 변수 map


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
