#include "Response.hpp"

// Constructors & Destructors

/*
 * 기본 생성자.
 * 외부에서 사용 못 하게 private으로 제한
 */
Response::Response()
{
}

/*
 * 복사 생성자
 */
Response::Response(const Response &other)
{
    *this = other;
}

/*
 * 사용할 수 있는 가장 기본적인 생성자
 * @param int statusCode: request를 처리한 후의 상태 코드
 * @param std::map<std::string, std::string> header: 헤더 데이터를 저장할 map
 * @param std::string body: request 처리 후 반환하는 본문
 * @throws UnsupportedStatusCodeException : statusCode 인자에 지원하지 않는 값을 넣으면 발생
 * @param std::string version: 지정할 HTTP 버전
 * 지원 Code :
 *   - 100 ~ 101
 *   - 200 ~ 206
 *   - 300 ~ 305, 307
 *   - 400 ~ 417
 *   - 500 ~ 505
 */
Response::Response(int statusCode, std::map<std::string, std::string> header,
        std::string body, std::string version)
    : version(version), statusCode(statusCode), header(header), body(body)
{
    switch (statusCode)
    {
        case 100:
            statusMessage = Response::_100;
            break ;
        case 101:
            statusMessage = Response::_101;
            break ;
        case 200:
            statusMessage = Response::_200;
            break ;
        case 201:
            statusMessage = Response::_201;
            break ;
        case 202:
            statusMessage = Response::_202;
            break ;
        case 203:
            statusMessage = Response::_203;
            break ;
        case 204:
            statusMessage = Response::_204;
            break ;
        case 205:
            statusMessage = Response::_205;
            break ;
        case 206:
            statusMessage = Response::_206;
            break ;
        case 300:
            statusMessage = Response::_300;
            break ;
        case 301:
            statusMessage = Response::_301;
            break ;
        case 302:
            statusMessage = Response::_302;
            break ;
        case 303:
            statusMessage = Response::_303;
            break ;
        case 304:
            statusMessage = Response::_304;
            break ;
        case 305:
            statusMessage = Response::_305;
            break ;
        case 307:
            statusMessage = Response::_307;
            break ;
        case 400:
            statusMessage = Response::_400;
            break ;
        case 401:
            statusMessage = Response::_401;
            break ;
        case 402:
            statusMessage = Response::_402;
            break ;
        case 403:
            statusMessage = Response::_403;
            break ;
        case 404:
            statusMessage = Response::_404;
            break ;
        case 405:
            statusMessage = Response::_405;
            break ;
        case 406:
            statusMessage = Response::_406;
            break ;
        case 407:
            statusMessage = Response::_407;
            break ;
        case 408:
            statusMessage = Response::_408;
            break ;
        case 409:
            statusMessage = Response::_409;
            break ;
        case 410:
            statusMessage = Response::_410;
            break ;
        case 411:
            statusMessage = Response::_411;
            break ;
        case 412:
            statusMessage = Response::_412;
            break ;
        case 413:
            statusMessage = Response::_414;
            break ;
        case 415:
            statusMessage = Response::_415;
            break ;
        case 416:
            statusMessage = Response::_416;
            break ;
        case 417:
            statusMessage = Response::_417;
            break ;
        case 500:
            statusMessage = Response::_500;
            break ;
        case 501:
            statusMessage = Response::_501;
            break ;
        case 502:
            statusMessage = Response::_502;
            break ;
        case 503:
            statusMessage = Response::_503;
            break ;
        case 504:
            statusMessage = Response::_504;
            break ;
        case 505:
            statusMessage = Response::_505;
            break ;
        default:
            throw UnsupportedStatusCodeException();
    }
}

/*
 * 소멸자
 */
Response::~Response()
{
}

// Operator Overloadings
/*
 * 할당 연산자 오버로딩
 * 깊은 복사 연산
 */
Response &Response::operator=(const Response &other)
{
    if (this != &other)
    {
        version = other.version;
        statusCode = other.statusCode;
        statusMessage = other.statusMessage;
        header = other.header;
        body = other.body;
    }
    return (*this);
}

// Exceptions
/*
 *
 */
const char *Response::UnsupportedStatusCodeException::what() const throw()
{
    return "[ERROR] UnsupportedStatusCodeException. Input status code is unsupported. Refer to RFC2616";
}

// Member Methods
/*
 * HTTP 리스폰스 메시지를 만들어서 리턴
 */
std::string Response::getMessage()
{
    std::string result;

    result = "HTTP/" + version + " " + ft_itoa(statusCode) + " " + statusMessage + "\r\n";
    for (std::map<std::string, std::string>::iterator iter = header.begin();
            iter != header.end(); iter++)
        result += iter->first + ": " + iter->second + "\n";
    result += "\r\n" + body + "\r\n";
    return (result);
}

const std::string    Response::_100 = "Continue";
const std::string    Response::_101 = "Switching Protocols";

const std::string    Response::_200 = "OK";
const std::string    Response::_201 = "Created";
const std::string    Response::_202 = "Accepted";
const std::string    Response::_203 = "Non-Authoritative Information";
const std::string    Response::_204 = "No Content";
const std::string    Response::_205 = "Reset Content";
const std::string    Response::_206 = "Partial Content";

const std::string    Response::_300 = "Multiple Choices";
const std::string    Response::_301 = "Moved Permanently";
const std::string    Response::_302 = "Found";
const std::string    Response::_303 = "See Other";
const std::string    Response::_304 = "Not Modified";
const std::string    Response::_305 = "Use Proxy";
const std::string    Response::_307 = "Temporary Redirect";

const std::string    Response::_400 = "Bad Request";
const std::string    Response::_401 = "Unauthorized";
const std::string    Response::_402 = "Payment Required";
const std::string    Response::_403 = "Forbidden";
const std::string    Response::_404 = "Not Found";
const std::string    Response::_405 = "Method Not Allowed";
const std::string    Response::_406 = "Not Accceptable";
const std::string    Response::_407 = "Proxy Authentication Required";
const std::string    Response::_408 = "Request Time-out";
const std::string    Response::_409 = "Conflict";
const std::string    Response::_410 = "Gone";
const std::string    Response::_411 = "Length Required";
const std::string    Response::_412 = "Precondition Failed";
const std::string    Response::_413 = "Request Entity Too Large";
const std::string    Response::_414 = "Request-URI Too Large";
const std::string    Response::_415 = "Unsupported Media Type";
const std::string    Response::_416 = "Requested range not satisfiable";
const std::string    Response::_417 = "Exception Failed";

const std::string    Response::_500 = "Internal Server Error";
const std::string    Response::_501 = "Not Implemented";
const std::string    Response::_502 = "Bad Gateway";
const std::string    Response::_503 = "Service Unavailable";
const std::string    Response::_504 = "Gateway Time-out";
const std::string    Response::_505 = "HTTP Version not supported";
