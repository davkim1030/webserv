#include "libft.h"
#include <iostream>

int ft_hex_atoi(const std::string &str)
{
	int result = 0;
	for (std::string::const_iterator iter = str.begin(); iter != str.end(); iter++)
	{
		if (*iter >= 'A' && *iter <= 'F')
			result = (*iter - 'A' + 10) + result * 16;
		else if (*iter >= 'a' && *iter <= 'f')
			result = (*iter - 'a' + 10) + result * 16;
		else if (*iter >= '0' && *iter <= '9')
			result = (*iter - '0') + result * 16;
		else break ;
	}
	return result;
}