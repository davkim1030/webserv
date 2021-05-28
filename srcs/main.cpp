/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seohchoi <seohchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/25 15:12:02 by hyukim            #+#    #+#             */
/*   Updated: 2021/05/28 15:34:07 by seohchoi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Request.hpp"

int		main(void)
{
	Request Req("GET /cgi-bin/http_trace.pl HTTP/1.1\r\na: b\r\nb: c\r\nc: d\r\n\r\n>_<");

	Req.parseRequest();
	std::cout << Req.getMethod() << std::endl;
	std::cout << Req.getUri() << std::endl;
	std::cout << Req.getHttpVersion() << std::endl;
	std::cout << "------" << std::endl << Req.getRawHeader() << std::endl << "------" << std::endl;

	std::map<std::string, std::string> header = Req.getHeader();
    std::map<std::string, std::string>::iterator iter;

    for( iter = header.begin(); iter != header.end(); iter++)
    {
        std::cout << "Key : " << iter->first << " Value : " << iter->second << std::endl;
    }

	std::cout << Req.getRawBody() << std::endl ;

	return (0);
}
