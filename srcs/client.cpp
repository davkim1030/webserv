#include "../includes/webserv.h"

#define PORT 8080
int main(int argc, char const *argv[])
{
    int clnt_sock;
    struct sockaddr_in serv_addr;
    char message[1024] = {0x00, };

    clnt_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (clnt_sock == -1)
        std::cout << "error : socket error" << std::endl;
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        std::cout << "error : connect error" << std::endl;
    if (read(clnt_sock, message, sizeof(message) - 1) == -1)
        std::cout << "error : read error" << std::endl;
    
    std::cout << "Message from server : {" << message << "}" << std::endl;

    close(clnt_sock);

    
    return 0;
}