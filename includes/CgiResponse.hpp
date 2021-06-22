#ifndef CGIRESPONSE_HPP
#define CGIRESPONSE_HPP

#include "ResponseMaker.hpp"
#include "Cgi.hpp"
#include "Resource.hpp"

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
    
		char** makeCgiEnvp();
		void cgiResponse();

};


#endif