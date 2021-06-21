#ifndef NORMALRESPONSE_HPP
#define NORMALRESPONSE_HPP

#include "ResponseMaker.hpp"

class NormalResponse : public ResponseMaker
{
	private:
		std::string resourcePath;

		NormalResponse();

	public:
		NormalResponse(const Request&, const Server&, const Location&);
		NormalResponse(const NormalResponse&);
		NormalResponse &operator=(const NormalResponse&);
		~NormalResponse();

		void		addLastModifiedHeader(std::string);
		void		addContentLanguageHeader(void);
		void		addContentLocationHeader(void);
		void		addAllowHeader(std::string);
		void		addHostHeader(void);

		int			checkPath(std::string);
		Response	makeResponse(void);
		std::string	parseResourcePath(std::string);
		void 		makeGetResponse(int);
		void 		makeHeadResponse(void);
		std::string	fileExtension(std::string);
		void 		makePostResponse(void);
		void 		makePutResponse(void);
		void 		makeDeleteResponse(void);
		void 		makeTraceResponse(void);
		void 		makeOptionResponse(void);
		void 		makeConnectResponse(void);
		std::string makeAutoIndexPage(std::string);

};

#endif