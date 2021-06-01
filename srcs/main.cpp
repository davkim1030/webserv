#include "webserv.h"

#define PORT 80

void operateIo(int maxSize, fd_set rfds, fd_set wfds, fd_set efds)
{
    int fd = 0;
	int opVal;
	fd_set tmpFds;
	char	buffer[100] = { 0, };
	size_t	bufferSize = 100;

    while (fd < maxSize)
    {
		std::cout << fd << std::endl;
		std::cout << FD_ISSET(fd, &rfds) << std::endl;
		FD_ZERO(&tmpFds);
        if (FD_ISSET(fd, &rfds))
        {
			opVal = read(fd, buffer, bufferSize);
			std::cout << opVal << std::endl;
			if (opVal >= 0)
			{
				// read file
				std::cout << "[read] " << buffer;
				ft_memset(buffer, '\0', 100);
			}
			else
			{
				// connection closed
				close(fd);
			}
			break ;
        }
        else if (FD_ISSET(fd, &wfds))
        {
			opVal = write(fd, buffer, bufferSize);
			if (opVal >= 0)
			{
				// read file
				std::cout << "[write] " << buffer;
				ft_memset(buffer, '\0', 100);
			}
			else
			{
				// connection closed
				close(fd);
			}
			break ;
        }
        else if (FD_ISSET(fd, &efds))
        {
			std::cout << "except fdset" << std::endl;
			break ;
        }
		fd++;
    }
}

int main(void)
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    ft_memset(address.sin_zero, '\0', sizeof address.sin_zero);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 5) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }

    fd_set readfds, writefds, exceptfds;
    while(1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_ZERO(&exceptfds);
		FD_SET(new_socket, &readfds);
		FD_SET(new_socket, &writefds);
		FD_SET(new_socket, &exceptfds);
        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        int selval = select(new_socket + 1, &readfds, &writefds, &exceptfds, &timeout);
        if (selval > 0)
        {
            // do I/O operation
			std::cout << selval << std::endl;
			operateIo(new_socket + 1, readfds, writefds, exceptfds);
        }
        else if (selval < 0)
        {
            // handle error
			std::cout << "nothing selected" << std::endl;
        }
        close(new_socket);
    }
	return (0);
}
