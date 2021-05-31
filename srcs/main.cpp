/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyukim <hyukim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/25 15:12:02 by hyukim            #+#    #+#             */
/*   Updated: 2021/05/25 15:14:28 by hyukim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.h"

#define PORT 8080
# define BUF_SIZE 100
# define CONFIG_PATH "./default.nginx"

// FD_ZERO(fd_set* set);        //fdset을초기화
// FD_SET(int fd, fd_set* set);  //fd를 set에 등록
// FD_CLR(int fd, fd_set* set);  //fd를 set에서 삭제
// FD_ISSET(int fd, fd_set* set);//fd가 준비되었는지 확인

void startServer()
{
		int serv_sock;
	int clnt_sock;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (serv_sock == -1)
		std::cout << "error : sock error" << std::endl;
	ft_memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT);

	if (bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == 1)
		std::cout << "error : bing error" << std::endl;
	
	if (listen(serv_sock, 5) == -1)
		std::cout << "error : listen error" << std::endl;

	fd_set reads;
	fd_set copyReads;

	fd_set writes;
	fd_set copyWrite;
	struct timeval timeout;

	int fd_max, str_len, fd_num;

	FD_ZERO(&reads); // fd_set 테이블 초기화
	FD_SET(serv_sock, &reads);

	fd_max = serv_sock;
	while(1)
	{
		char	buf[BUF_SIZE];
		copyReads = reads;
		copyWrite = writes;
		timeout.tv_sec = 5;
		timeout.tv_usec = 5000;

		if ((fd_num = select(fd_max + 1, &copyReads, &copyWrite, 0, &timeout)) == -1 )
		{
			std::cout << "select error" << std::endl;
			exit(0);
		}
		if (fd_num == 0)
			continue;

		for (int i = 0; i <= fd_max; i++)
		{
			if (FD_ISSET(i, &copyReads))
			{
				if (i == serv_sock)
				{
					clnt_addr_size = sizeof(clnt_addr);
					clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
					if (clnt_sock == -1)
						std::cout << "error : client accept error" << std::endl;
					FD_SET(clnt_sock, &reads);
					FD_SET(clnt_sock, &writes);
					if (fd_max < clnt_sock)
						fd_max = clnt_sock;

					//클라이언트 연결만 허용
					
					char message[] = "Hello test world\n";
					write(clnt_sock, message, sizeof(message));
				}
				else
				{
					str_len = read(i, buf, BUF_SIZE);
					// str_len == 0
					// 	disconnnect
					
					// 클라이언트 인풋 read
					// 처리

					//result -> 클라이언트에 write
					FD_CLR(i, &reads);
					close(i);
					std::cout << "close file : " << i << std::endl;
				}
			}
			else if (FD_ISSET(i, &copyWrite))
			{
				// write(clnt_sock, buf, size);
			}
		}

	}
	close(clnt_sock);
	close(serv_sock);
}

class Location
{
	private:
		std::map<std::string, std::string> option;


	public:
		Location() {}
		Location(Location const &co) {}
		Location &operator=(Location const &co)
		{
			return *this;
		}
		~Location() {}
};

class Server
{
	private:
		std::map<std::string, std::string> option;
		std::vector<Location> location;


	public:
		//std::string 형태로 인자 받아서 바로 파싱해서 저장하자
		Server() {}
		Server(const std::string &str)
		{
		}
		Server(Server const &co) : option(co.option), location(co.location)
		{}
		Server &operator=(Server const &co) 
		{
			option = co.option;
			location = co.location;
			return *this;
		}
		~Server() {}
};

class ServerConfig
{
	private:
		std::map<std::string, std::string> option; // 기본 option 저장
		std::vector<Server> server;

	public:
		ServerConfig() {}
		ServerConfig(ServerConfig const &co) : option(co.option), server(co.server)
		{}
		ServerConfig &operator=(ServerConfig const &co)
		{
			option = co.option;
			server = co.server;
			return *this;
		}
		~ServerConfig() {}

		// 
		int checkLine(std::string line)
		{
			int i = 0;
			int res = 1;
			while (line[i])
			{
				if (ft_isalnum(line[i]))
					res = 0;
				i++;
			}
			return res;
		}

		// option 파일의 option 저장
		void configParse(std::string line)
		{
			int index;
			if ((index = line.find('#')) != -1)
			{
				line = line.substr(0, index);
			}
			if (checkLine(line))
				return ;
			std::pair<std::string, std::string> res;
			line = line.substr(0, line.find(';'));

			char *test = std::strtok((char *)line.c_str(), " \t");
			if (test == NULL)
				return ;
			res.first = std::string(test);

			test = std::strtok(NULL, " \t");
			line.clear();
			while (test != 0)
			{
				line.append(test);
				test = std::strtok(NULL, " \t");
				if (test != 0)
					line.append(" ");
			}
			res.second = line;
			if (res.second.empty() || res.first.empty())
				return ;
			option[res.first] = res.second; // 덮어쓸 땐 이걸로 바꾸면 됨
		}

		void saveConfig(int fd)
		{
			int parenFlag = 0;
			char *line;
			std::string stdline;
			while (get_next_line(fd, &line))
			{
				std::string test(line);
				if (test.find('{') != -1)
				{
					stdline.append(test + '\n');
					parenFlag++;
				}
				else if (test.find('}') != -1)
				{
					stdline.append(test + '\n');
					parenFlag--;
					if (parenFlag == 0)
					{
						server.push_back(Server(stdline));
						stdline.clear();
					}
				}
				else if (parenFlag > 0)
				{
					stdline.append(test + '\n');
				}
				else
				{
					configParse(test);
				}
				// flag == 0 이고 줄에 중괄호가 없으면 config에 값 저장
				// 줄에 { -> 중괄호 시작이 있으면 flag += 1 && line저장 시작
				// flag > 0 이고 줄에 중괄호가 없으면 line 저장 후 한번에 하위 저장클래스에 넘겨주기
				// flag > 0 이고 중괄호 } 가 나오면 flag -= 1 이 때 flag 가 0이되면 이대로 끝내서 하위클래스에 패스
				free(line);
			}

			std::map<std::string, std::string>::iterator it;

			for(it = option.begin(); it != option.end(); it++)
				std::cout << "{" << it->first << "} {" << it->second << "}" << std::endl;
		}

};

int		main(int argc, char **argv)
{
	ServerConfig serverConfig;

	std::vector<int> ser;

	ser.push_back(2);

	char *line;
	if (argc > 2)
	{
		std::cout << "Error : argument error" << std::endl;
		exit(1);
	}
	int configFd;
	if (argc == 2)
		configFd = open(argv[1], O_RDONLY);
	else
		configFd = open(CONFIG_PATH, O_RDONLY);
	
	serverConfig.saveConfig(configFd);



	return (0);
}