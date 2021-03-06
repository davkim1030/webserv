#ifndef CGIRESPONSE_HPP
# define CGIRESPONSE_HPP

# include "ResponseMaker.hpp"
# include "CgiWriter.hpp"
# include "Resource.hpp"

#define CGI_DIR		"./cgiFiles/"	// cgi의 결과인 임시 파일을 저장할 디렉토리
#define CGI_PATH	"cgiRes"		// cgi 임시 파일이름 접두어

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
		
		bool	makeVariable(int clientFd);
		char**	makeCgiEnvp();
		void	cgiResponse(int clientFd);

		static	Response	cgiResultParsing(std::string cgiResult);
};
#endif
