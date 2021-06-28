#include "CgiResponse.hpp"
#include "Socket.hpp"
#include "CgiResource.hpp"
#include "Client.hpp"

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
	cgi 실행 파일에 필요한 변수들을(meta variable) 만들어주는 함수
	인자로 들어갈 파일의 유효성을 검사하고 유효하지 않을 경우 에러를 띄워줌
	@param int clientFd : 상태 변환을 줘야 할 client의 fd
	@return 인자로 들어가야 할 파일이 있는지 확인 후 없으면 false줌
*/
bool	CgiResponse::makeVariable(int clientFd)
{
	std::map<std::string, std::string> reqHead = request.getHeader();

	for (std::map<std::string, std::string>::iterator it = reqHead.begin(); it != reqHead.end(); it++)
		metaVariable["HTTP_" + ftToupperStr(it->first)] = it->second;

	std::string path = location.getPath();
	if (*path.rbegin() == '/')
		path = path.substr(0, path.length() - 1);
	std::string uri = request.getUri().substr(path.length());
	std::vector<std::string> extensions = location.getCgiExtensionVector();
	for (std::vector<std::string>::iterator it = extensions.begin(); it != extensions.end(); it++)
	{
		size_t index = uri.find(*it);
		if (index != std::string::npos && uri.compare(index, it->length(), *it) == 0)
		{
			int queryIndex;
			if ((queryIndex = uri.find('?')) != -1)
			{
				metaVariable["QUERY_STRING"] = uri.substr(queryIndex + 1);
				uri = uri.substr(0, queryIndex);
			}
			metaVariable["SCRIPT_NAME"] = uri.substr(0, index + it->length());
			uri = uri.substr(index);
			size_t pathIndex = uri.find('/');
			if (pathIndex != std::string::npos)
			{
				metaVariable["PATH_INFO"] = uri.substr(pathIndex);
				uri = uri.substr(0, pathIndex);
			}
		}
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

	// metaVariable["REQUEST_URI"] = request.getUri();
	metaVariable["REQUEST_URI"] = metaVariable["PATH_INFO"];
	if (metaVariable.count("PATH_INFO") == 1)
		metaVariable["REQUEST_URI"] = metaVariable["PATH_INFO"];

	metaVariable["PATH_TRANSLATED"] = location.getOption("root") + metaVariable["PATH_INFO"].substr(1);
	metaVariable["REMOTE_ADDR"] = server.getIp();

	metaVariable["REQUEST_METHOD"] = "GET";
	metaVariable["SERVER_NAME"] = "BSJ";

	char *port = ft_itoa(server.getPort());
	metaVariable["SERVER_PORT"] = std::string(port);
	free(port);

	metaVariable["SERVER_PROTOCOL"] = "HTTP/1.1";
	metaVariable["SERVER_SOFTWARE"] = "joockim/1.1";

	std::string filePath = location.getOption("root");
	filePath = filePath.substr(0, filePath.length() - 1) + metaVariable["SCRIPT_NAME"];
	

	if (checkPath(filePath) == NOT_FOUND || checkPath(filePath) == ISDIR)
	{
		updateErrorStatus(clientFd, 404);
		return false;
	}
	return true;
}

/*
	저장된 meta variable 들을 envp 형태로 가공해주는 함수
	@return 만들어진 envp 포인터
*/
char** CgiResponse::makeCgiEnvp()
{
	char **res = (char **)malloc(sizeof(char *) * metaVariable.size() + 1);
	int i = 0;
	for (std::map<std::string, std::string>::const_iterator it = metaVariable.begin();
	it != metaVariable.end(); it++)
	{
		std::string temp = std::string(it->first + "=" + it->second);
		res[i] = ft_strdup(temp.c_str());
		i++;
	}
	res[i] = NULL;
	return res;
}

/*
	cgi 프로그램을 실행 해주는 함수
	fork()를 이용해 프로세스를 나눈 후 자식 프로세스에서 execve()를 통해 실행시켜 줌
	@param int clientFd : 상태 변환을 줘야 할 client의 fd
*/
void CgiResponse::cgiResponse(int clientFd)
{
	pid_t pid;
	int fd[2];

	if (pipe(fd) == -1)
		return updateErrorStatus(clientFd, 500);
	int fd_read = fd[0];
	int fd_write = fd[1];

	fcntl(fd[1], F_SETFL, O_NONBLOCK);

	int checkRes = checkPath("./cgifiles");
	if (checkRes == ISFILE)
		return updateErrorStatus(clientFd, 500);
	else if (checkRes == NOT_FOUND && mkdir(CGI_DIR, 0766) == -1)
		return updateErrorStatus(clientFd, 500);

	char *num = ft_itoa(clientFd);
	std::string tempFile = std::string(CGI_DIR) + std::string(CGI_PATH) + "_" + std::string(num);
	free(num);
	int fd_temp = open(tempFile.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0666);
	if (fd_temp == -1)
		return updateErrorStatus(clientFd, 500);
	if ((pid = fork()) == -1)
		return updateErrorStatus(clientFd, 500);
	else if (pid == 0)
	{
		int execveRet;
		close(fd_write);
		dup2(fd_read, 0);
		dup2(fd_temp, 1);
		char *argv[3];
		argv[0] = strdup(location.getOption("cgi_path").c_str());
		std::string root = location.getOption("root");
		root = root.substr(0, root.length() - 1);
		argv[1] = strdup((root + metaVariable["SCRIPT_NAME"]).c_str());
		argv[2] = NULL;
		char **envp = makeCgiEnvp();
		execveRet = execve(argv[0], argv, envp);
		char **temp = envp;
		while (*envp)
			free(*envp++);
		free(temp);
		exit(execveRet);
	}
	else
	{
		close(fd_read);

		CgiWriter *pipe = new CgiWriter(fd_write, clientFd);
		Socket::getInstance()->getPool()[fd_write] = pipe;
		Socket::getInstance()->updateFds(fd_write, FD_WRITE);

		CgiResource *resource = new CgiResource(fd_temp, pid, clientFd);
		Socket::getInstance()->getPool()[fd_temp] = resource;
		Socket::getInstance()->updateFds(fd_temp, FD_READ);

		Socket::getInstance()->updateFdMax();
		return ;
	}

}

/*
	cgi 실행파일에서 나온 결과물을 response message로 만들어주는 함수
	@param std::string cgiResult : cgi 실행파일이 내뱉은 결과물을 저장 한 문자열
	@return 완성된 response 메시지를 담은 Response 객체
*/
Response	CgiResponse::cgiResultParsing(std::string cgiResult)
{
	int statusCode;
	std::map<std::string, std::string> header;
	std::string body;
	std::string tmp;

	tmp = cgiResult.substr(cgiResult.find("Status: ") + 8, 3);
	// ftLog("tmp ", tmp);
	statusCode = ft_atoi(tmp.c_str());
	tmp = cgiResult.substr(cgiResult.find("\r\n") + 2);
	while (tmp.compare(0, 2, "\r\n") != 0)
	{
		std::string line = tmp.substr(0, tmp.find("\r\n"));
		if (line.length() == 0)
			break ;
		else
		{
			std::string key = line.substr(0, line.find(": "));
			std::string value = line.substr(line.find(": ") + 2);
			header[key] = value;
		}
		tmp = tmp.substr(tmp.find("\r\n") + 2);
	}
	body = tmp.substr(2);
	return Response(statusCode, header, body);
}
