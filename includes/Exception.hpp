#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <iostream>

class WrongFileFormat : public std::exception
{
	const char *what() const throw();
};

class FileDoesNotExist : public std::exception
{
    const char *what() const throw();
};


#endif