#ifndef LOCATION_HPP
#define LOCATION_HPP

# include <map>
# include <string>

/*
 *location 블럭 저장 클래스
 */
class Location
{
	private:
		std::string							path;	// location의 경로
		std::map<std::string, std::string>	option;	// location이 가질 기본 정보 저장
		std::vector<std::string> index;				// 인덱스 파일 이름들 
		std::vector<std::string> allow_method;		// 허용된 리퀘스트 메소드 목록
		std::vector<std::string> cgi_extension;		// 지원하는 cgi 확장자 목록

	public:
		Location();
		Location(Location const &lo);
		Location(std::list<std::string> &line);
		Location &operator=(Location const &lo);
		~Location();

		std::vector<std::string>& getIndexVector();
		std::vector<std::string>& getAllowMethodVector();
		std::vector<std::string>& getCgiExtensionVector();

		std::string getOption(std::string const &key);
		std::string getPath() const;

		void configParse(std::string line);
		void saveLocation(std::string line);
		void printItem();
};
#endif
