#include <iostream>
#include <string.h>
#include <map>
# include <fcntl.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <signal.h>
# include <string.h>
# include <sys/errno.h>
# include <arpa/inet.h>
# include <sys/stat.h>
# include <dirent.h>
# include <sys/time.h>
# include <time.h>
# include <sys/select.h>
# include <sys/socket.h>

char** cgiRequest()
{
	std::map<std::string, std::string> metaVariable;

	metaVariable["AUTH_TYPE"] = "null";
	metaVariable["CONTENT_LENGTH"] = "100";
	metaVariable["CONTENT_TYPE"] = "MIME type";
	metaVariable["GATEWAY_INTERFACE"] = "CGI/1.1";

	// /test/cgi.bla/path/?query_string -> 이렇게 들어올 수 있음
	// 뒤에 아무것도 안들어오는 경우, /만 들어오고 끝나는 경우, /path 하고 들어오는 경우
	//   uri 통째로 써주기 ,      /만 써주기,             /path 써주기
	metaVariable["PATH_INFO"] = "/";
	metaVariable["PATH_TRANSLATED"] = "root/PATH_INFO";
	metaVariable["QUERY_STRING"] = "QUERY_STRING";

	metaVariable["REMOTE_ADDR"] = "clientIP"; // 클라이언트의 IP
	metaVariable["REMOTE_IDENT"] = ""; // 없어두됨
	metaVariable["REMOTE_USER"] = ""; // 없어두됨 REMOTE 둘 다 AUTH 파일에 넘겨져 온다고 생각 중

	metaVariable["REQUEST_METHOD"] = "GET";
	metaVariable["REQUEST_URI"] = "URI";
	metaVariable["SCRIPT_NAME"] = ".bla file";
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
		res[i] = strdup(temp.c_str());
		i++;
	}
	res[i] = NULL;
	return res;
}

void error_exit(std::string str)
{
	std::cout << str << std::endl;
	exit(1);
}

int main()
{
	pid_t pid;
	char **envp = cgiRequest();

	int fd[2];

	if (pipe(fd) == -1)
		error_exit("pipe fail");

	int fd_read = fd[0];
	int fd_write = fd[1];

	std::string file_name = "./temp_file";
	int fd_temp = open(file_name.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0666);
	if (fd_temp == -1)
		error_exit("temp file exit");
	if ((pid = fork()) == -1) // fork error
		error_exit("fork fail");
	else if (pid == 0) // child process
	{
		std::cout << "child " << std::endl;
		close(fd_write);
		dup2(fd_read, 0);
		dup2(fd_temp, 1);
		char *argv[3];
		argv[0] = strdup("cgi_tester");
		argv[1] = strdup("test.bla");
		argv[2] = NULL;
		int ret;
		ret = execve(argv[0], argv, envp);
		close(fd_temp);
		exit(ret);	
	}
	else // parent process
	{
		int status;
		int s = waitpid(pid, &status, 0);
		close(fd_read);
		//dup2(fd_write, 1);
		
		lseek(fd_temp, 0, SEEK_SET);
		char buf[1000];
		bzero(buf, 1000);
		int res = read(fd_temp, buf, 1000);
		std::cout << "buf : {" << buf << "}" << std::endl;
		std::cout << "res : " << res << " | status : " << status << std::endl;
		std::cout << "error code : " << ESPIPE << std::endl;
	}


	return 0;
}





