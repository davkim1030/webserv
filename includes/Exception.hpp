#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <iostream>

class WrongFileFormatException : public std::exception
{
	const char *what() const throw();
};

class FileDoesNotExistException : public std::exception
{
    const char *what() const throw();
};


#endif
