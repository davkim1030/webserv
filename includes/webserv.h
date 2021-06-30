#ifndef WEBSERV_H
# define WEBSERV_H

# include <iostream>
# include <string>
# include <vector>
# include <map>
# include <stack>
# include <queue>
# include <list>
# include <algorithm>
# include <exception>

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

# include "libft.h"

# define CONFIG_PATH "./default.nginx"

# define NOT_FOUND 404
# define SERVER_ERR 500
# define FORBIDDEN 403
# define METHOD_NOT_ALLOWED 405
# define REQUEST_ENTITY_TOO_LARGE 413
# define OK 200
# define CREATED 201

# define NOT_EXIST 0
# define ISFILE 1
# define ISDIR 2

#endif
