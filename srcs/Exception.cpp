#include "Exception.hpp"

/*
    file이 올바르지 않은 포맷으로 이루어져 있을 경우 사용
*/
const char *WrongFileFormatException::what() const throw()
{
	return "Error : wrong file format";
}

/*
    파일을 열지 못했을 때 또는 존재하지 않는 파일일 경우 사용
*/
const char *FileDoesNotExistException::what() const throw()
{
    return "Error : file not exist";
}

/*
 * 실행시 argument의 갯수나 내용일 틀릴시 발생시키는 예외
 */
const char *ArgumentException::what() const throw()
{
    return "Error : wrong argument. ./webserv [CONFIG_FILE]";
}
