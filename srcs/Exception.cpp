#include "Exception.hpp"

/*
    file이 올바르지 않은 포맷으로 이루어져 있을 경우 사용
*/
const char *WrongFileFormat::what() const throw()
{
	return "Error : wrong file format";
}

/*
    파일을 열지 못했을 때 또는 존재하지 않는 파일일 경우 사용
*/
const char *FileDoesNotExist::what() const throw()
{
    return "Error : file not exist";
}