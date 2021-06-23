#ifndef NORMALRESPONSE_HPP
# define NORMALRESPONSE_HPP

# include "ResponseMaker.hpp"
# include "Resource.hpp"

/*
 * cgi가 아닌 일반적인 리스폰스를 담당하는 클래스
 */
class NormalResponse : public ResponseMaker
{
	private:
		std::string resourcePath;	// TODO: 주석 달기
		std::string resource;		// TODO: 주석 달기

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
		
		Response	makeResponse(const std::string &resource);
		std::string	parseResourcePath(std::string);
		void 		makeGetResponse(int);
		void 		makeHeadResponse(void);
		std::string	fileExtension(std::string);
		/*void 		makePostResponse(void);
		void 		makePutResponse(void);*/
		void 		makeDeleteResponse(void);
		void 		makeTraceResponse(void);
		void 		makeOptionResponse(void);
		void 		makeConnectResponse(void);
		std::string makeAutoIndexPage(std::string);

};
#endif