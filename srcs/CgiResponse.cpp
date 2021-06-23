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

void	CgiResponse::makeVariable(int clientFd)
{
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

	metaVariable["REQUEST_URI"] = request.getUri();
	if (metaVariable.count("PATH_INFO") == 1)
		metaVariable["REQUEST_URI"] = metaVariable["PATH_INFO"];

	metaVariable["PATH_TRANSLATED"] = location.getOption("root") + metaVariable["PATH_INFO"].substr(1);
	metaVariable["REMOTE_ADDR"] = server.getIp();

	metaVariable["REQUEST_METHOD"] = "GET";
	metaVariable["SERVER_NAME"] = "BSJ_we are never die";

	char *port = ft_itoa(server.getPort());
	metaVariable["SERVER_PORT"] = std::string(port);
	free(port);

	metaVariable["SERVER_PROTOCOL"] = "HTTP/1.1";
	metaVariable["SERVER_SOFTWARE"] = "joockim/1.1";

	std::string filePath = location.getOption("root");
	filePath = filePath.substr(0, filePath.length() - 1) + metaVariable["SCRIPT_NAME"];

	if (checkPath(filePath) == NOT_FOUND || checkPath(filePath) == ISDIR)
	{
		// error
	}


	// TODO : 파일 존재 유무 확인하기 -> 어떤 파일? cgi에 넣을 .bla 파일


}

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

void CgiResponse::cgiResponse(int clientFd)
{
	pid_t pid;
	int fd[2];

	if (pipe(fd) == -1)
		return updateErrorStatus(clientFd, 500);
	int fd_read = fd[0];
	int fd_write = fd[1];

	fcntl(fd[1], F_SETFL, O_NONBLOCK);

	int checkRes = checkPath("./cgi_files");
	std::cout << checkRes << std::endl;
	if (checkRes != ISFILE)
	{
		return updateErrorStatus(clientFd, 404);
	}
	else if (checkRes == NOT_FOUND)
	{
		if (mkdir(CGI_DIR, 0766) == -1)
		{
			Socket::getInstance()->getPool()[clientFd]->setStatus(PROCESSING_ERROR);
			return ;
		}
	}
	char *num = ft_itoa(clientFd);
	std::string tempFile = std::string(CGI_DIR) + std::string(CGI_PATH) + std::string(num);
	free(num);
	// TODO: 다 읽고 파일 지워야 함
	int fd_temp = open(tempFile.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0666);
	if (fd_temp == -1)
	{
		Socket::getInstance()->getPool()[clientFd]->setStatus(PROCESSING_ERROR);
		return ;
	}
	if ((pid = fork()) == -1)
	{
		Socket::getInstance()->getPool()[clientFd]->setStatus(PROCESSING_ERROR);
		return ;
	}
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
		execveRet = execve(argv[0], argv, makeCgiEnvp());
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

Response	CgiResponse::cgiResultPasring(std::string cgiResult)
{
	int statusCode;
	std::map<std::string, std::string> header;
	std::string body;
	std::string tmp;

	tmp = cgiResult.substr(cgiResult.find("Status: ") + 8, 3);
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
