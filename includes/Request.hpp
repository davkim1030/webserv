#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <string>
# include <map>


class Request
{

	public:

		Request();
		Request( Request const & src );
		~Request();

		Request &		operator=( Request const & rhs );

		static const std::string header_list[];

	private:
		std::string	raw_request;

		std::string	method;
		std::string	uri;
		std::string	http_version;

		std::string raw_header;
		std::map<std::string, std::string> header;

		std::string	raw_body;

		int	status;
		int	type;
		void	parse_request(void);

};



#endif /* ********************************************************* REQUEST_H */