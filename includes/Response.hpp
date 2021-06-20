#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.h"

/*
 * HTTP response를 처리하는 클래스
 * 헤더 데이터를 map<string, string> 형태로 받음
 */
class Response
{
private:
    std::string version;                        // 리스폰스 버전
    int statusCode;                             // 상태 코드
    std::string statusMessage;                  // 상태 메시지
    std::map<std::string, std::string> header;  // 헤더 데이터
    std::string body;                           // 바디 문자열
    int lastResponse;                           // 마지막 리스폰스 타입

    Response();

public:
    // Constants
    /*
     * HTTP 상태 코드
     */
    static const std::string    _100;   // Continue
    static const std::string    _101;   // Switching Protocols

    static const std::string    _200;   // OK
    static const std::string    _201;   // Created
    static const std::string    _202;   // Accepted
    static const std::string    _203;   // Non-Authoritative Information
    static const std::string    _204;   // No Content
    static const std::string    _205;   // Reset Content
    static const std::string    _206;   // Partial Content

    static const std::string    _300;   // Multiple Choices
    static const std::string    _301;   // Moved Permanently
    static const std::string    _302;   // Found
    static const std::string    _303;   // See Other
    static const std::string    _304;   // Not Modified
    static const std::string    _305;   // Use Proxy
    static const std::string    _307;   // Temporary Redirect

    static const std::string    _400;   // Bad Request
    static const std::string    _401;   // Unauthorized
    static const std::string    _402;   // Payment Required
    static const std::string    _403;   // Forbidden
    static const std::string    _404;   // Not Found
    static const std::string    _405;   // Method Not Allowed
    static const std::string    _406;   // Not Acceptable
    static const std::string    _407;   // Proxy Authentication Required
    static const std::string    _408;   // Request Time-out
    static const std::string    _409;   // Conflict
    static const std::string    _410;   // Gone
    static const std::string    _411;   // Length Required
    static const std::string    _412;   // Precondition Failed
    static const std::string    _413;   // Request Entity Too Large
    static const std::string    _414;   // Request-URI Too Large
    static const std::string    _415;   // Unsupported Media Type
    static const std::string    _416;   // Requested range not satisfiable
    static const std::string    _417;   // Exception Failed

    static const std::string    _500;   // Internal Server Error
    static const std::string    _501;   // Not Implemented
    static const std::string    _502;   // Bad Gateway
    static const std::string    _503;   // Service Unavailable
    static const std::string    _504;   // Gateway Time-out
    static const std::string    _505;   // HTTP Version not supported

    /*
     * 지원하지 않는 상태코드를 넣을 경우 발생되는 예외
     */
    class UnsupportedStatusCodeException : public std::exception
    {
    public:
        virtual const char *what() const throw();
    };

    // Constructors & destructor
    Response(const Response &other);
    Response(int statusCode, std::map<std::string, std::string> header,
            std::string body, std::string version="HTTP/1.1");
    ~Response();

    // Operator overloading
    Response &operator=(const Response &other);

    // Methods
    std::string getMessage();
    int getStatusCode();
    int getLastResponse();

    void setLastResponse(int lastResponse);
    void initResponse();

};
#endif
