#ifndef RESPONSEHANDLER_HPP
# define RESPONSEHANDLER_HPP

# include "ResponseMaker.hpp"
# include "Resource.hpp"

/*
 * cgi가 아닌 일반적인 요청의 리스폰스를 처리하는 클래스
 */
class ResponseHandler : public ResponseMaker
{
	private:
		std::string resourcePath;	// resource의 경로
		std::string resource;		// resource의 내용
		int clientFd;

		ResponseHandler();

	public:
		ResponseHandler(const Request&, const Server&, const Location&, const std::string &buffer);
		ResponseHandler(const ResponseHandler&);
		ResponseHandler &operator=(const ResponseHandler&);
		~ResponseHandler();

		void		addLastModifiedHeader(std::string);
		void		addContentLanguageHeader(void);
		void		addContentLocationHeader(void);
		void		addAllowHeader(std::string);
		void		addHostHeader(void);
		
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
