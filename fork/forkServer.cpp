#include "netWorkUtil.h"
#include <iostream>

int main()
{
    TcpServer server(INADDR_ANY, 8000, false);

    while (1)
    {
        sockaddr clientAddr;
        int clientSock = server.Accept(clientAddr);
        if (clientSock < 0) {
            std::cout << "accept error" << std::endl;
            continue;
        }
        int pid = fork();
        if (pid > 0) {
            // parent
            close(clientSock);
        } else if (pid == 0){
            // child
            char buffer[1024];
            while(1) {
                int ret = read(clientSock, buffer, 1024);
                if (ret == 0) {
                    std::cout << "connection close" << std::endl;
                    close(clientSock);
                    exit(0);
                } else if (ret < 0) {
                    std::cout << "read error" << std::endl;
                } else {
                    send(clientSock, buffer, ret, 0);
                }
            }
        } else {
            std::cout << "fork error" << std::endl;
            exit(0);
        }
    }
    
}