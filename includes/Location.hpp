#ifndef LOCATION_HPP
#define LOCATION_HPP

# include <map>
# include <string>


class Location
{
	private:
		std::map<std::string, std::string> option;


	public:
		Location();
		Location(Location const &lo);
		Location &operator=(Location const &lo);
		~Location();
};

#endif