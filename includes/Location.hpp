#ifndef LOCATION_HPP
#define LOCATION_HPP

# include <map>
# include <string>

/*
	location 블럭 저장 클래스
	@member variable -> path : location의 경로
	@member variable -> option : location이 가질 기본 정보 저장
*/
class Location
{
	private:
		std::string							path;
		std::map<std::string, std::string>	option;


	public:
		Location();
		Location(Location const &lo);
		Location(std::list<std::string> &line);
		Location &operator=(Location const &lo);
		~Location();

		void configParse(std::string line);
		void saveLocation(std::string line);

		void printItem();

		std::string getOption(std::string const &key);
		std::string getPath() const;
		
};

#endif
