#include "CgiResponse.hpp"
#include "Socket.hpp"

// CgiResponse::CgiResponse() {}

CgiResponse::CgiResponse(const CgiResponse &cg)
: ResponseMaker(cg)
{}

CgiResponse &CgiResponse::operator=(const CgiResponse &cg)
{
    if (this != &cg)
    {
        ResponseMaker::operator=(cg);
    }
    return *this;
}

CgiResponse::CgiResponse(const Request& request, const Server& server, const Location& location)
: ResponseMaker(request, server, location)
{}

CgiResponse::~CgiResponse()
{}



/*
	cgi 처리 단계
	1. 들어온 파일 확인해서 cgi 확장자인지 확인
	2. cgi 이름에 맞는 파일이 있는지 찾기 ex) test.bla 들어오면 cgi_test 찾기
	3. uri 내부에 ?가 있는지 확인 후 ? 뒤에 있는 정보들은 query_string으로 저장
	4. 기본 환경변수 목록 만들기 -> map으로 저장해두면 편할 것 같음
	5. 저장된 환경변수 char** 형태로 가공
	6. 완성된 환경변수를 이용 해 cgi call
		6-1. pipe 생성 후 fcntl로 파일 속성 변경 (NON_BLOCK)
		6-2. fork로 프로세스 분리
		6-3. 가공된 정보를 통해 cgi 실행
		6-4. execve(실행할 파일 이름, (여긴 좀 더 알아볼 것_), 가공해둔 envp)
	7. execve에서 실행된 결과를 pipe를 통해 가져와서 body로 넘겨주기


*/
char** CgiResponse::makeCgiEnvp()
{
	std::string uri = request.getUri().substr(location.getPath().length());
	std::vector<std::string> extensions = location.getCgiExtensionVector();
	for (std::vector<std::string>::iterator it = extensions.begin(); it != extensions.end(); it++)
	{
		
	}


	if (metaVariable["PATH_INFO"].empty())
		metaVariable["PATH_INFO"] = request.getUri();

	metaVariable["AUTH_TYPE"] = request.getHeader()["Authorization"];
	if (metaVariable["AUTH_TYPE"].empty())
		metaVariable["AUTH_TYPE"] = "null";

	metaVariable["CONTENT_LENGTH"] = request.getHeader()["Content-Length"];
	if (metaVariable["CONTENT_LENGTH"].empty())
		metaVariable["CONTENT_LENGTH"] = "0";

	metaVariable["CONTENT_TYPE"] = request.getHeader()["Content-Type"];
	if (metaVariable["CONTENT_TYPE"].empty())
		metaVariable["CONTENT_TYPE"] = "null";
	metaVariable["GATEWAY_INTERFACE"] = "Cgi/1.1";

	metaVariable["REQUEST_URI"] = request.getUri();
	if (metaVariable.count("PATH_INFO") == 1)
		metaVariable["REQUEST_URI"] = metaVariable["PATH_INFO"];

	// /test/cgi.bla/path/?query_string -> 이렇게 들어올 수 있음
	// 뒤에 아무것도 안들어오는 경우, /만 들어오고 끝나는 경우, /path 하고 들어오는 경우
	//   uri 통째로 써주기 ,      /만 써주기,             /path 써주기

	metaVariable["PATH_TRANSLATED"] = location.getOption("root") + metaVariable["PATH_INFO"].substr(1);
	metaVariable["REMOTE_ADDR"] = "127.0.0.1"; // 클라이언트의 IP
	// metaVariable["REMOTE_IDENT"] = ""; // 없어두됨
	// metaVariable["REMOTE_USER"] = ""; // 없어두됨 REMOTE 둘 다 AUTH 파일에 넘겨져 온다고 생각 중

	metaVariable["REQUEST_METHOD"] = "GET";
	metaVariable["SERVER_NAME"] = "my server name";
	metaVariable["SERVER_PORT"] = "server port";
	metaVariable["SERVER_PROTOCOL"] = "HTTP/1.1";
	metaVariable["SERVER_SOFTWARE"] = "joockim/1.1";


	char **res = (char **)malloc(sizeof(char *) * metaVariable.size() + 1);
	int i = 0;
	for (std::map<std::string, std::string>::const_iterator it = metaVariable.begin();
	it != metaVariable.end(); it++)
	{
		std::string temp = std::string(it->first + "=" + it->second);
		std::cout << temp << std::endl;
		res[i] = ft_strdup(temp.c_str());
		i++;
	}
	res[i] = NULL;
	return res;
}

void CgiResponse::cgiResponse()
{
	pid_t pid;

	int fd[2];
	if (pipe(fd) == -1)
		throwErrorResponse(500, request.getHttpVersion());
	int fd_read = fd[0];
	int fd_write = fd[1];

	fcntl(fd[1], F_SETFL, O_NONBLOCK);

	std::string tempFile = "test_file";
	int fd_temp = open(tempFile.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0666);
	if (fd_temp == -1)
		throwErrorResponse(500, request.getHttpVersion());

	if ((pid = fork()) == -1)
		throwErrorResponse(500, request.getHttpVersion());
	else if (pid == 0)
	{
		int execveRet;
		std::cout << "child " << std::endl;
		close(fd_write);
		dup2(fd_read, 0);
		dup2(fd_temp, 1);
		char *argv[3];
		argv[0] = strdup(location.getOption("cgi_path").c_str());
		argv[1] = strdup((location.getOption("root") + metaVariable["SCRIPT_NAME"]).c_str());
		argv[2] = NULL;
		execveRet = execve(argv[0], argv, makeCgiEnvp());
		exit(execveRet);
	}
	else
	{
		close(fd_read);

		Cgi *cgi = new Cgi(fd_write, pid);
		Socket::getInstance()->getPool()[fd_write] = cgi;
		Socket::getInstance()->updateFds(fd_write, FD_WRITE);

		Resource *resource = new Resource(fd_temp);
		Socket::getInstance()->getPool()[fd_temp] = resource;
		Socket::getInstance()->updateFds(fd_write, FD_READ);

		Socket::getInstance()->updateFdMax();
		return ;
	}

}
