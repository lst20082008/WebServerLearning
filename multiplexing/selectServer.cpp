#include "netWorkUtil.h"
#include <iostream>
#include <algorithm>

int ReadAndSendBack(int clientSock)
{
    char buffer[1024];
    int ret = read(clientSock, buffer, 1024);
    if (ret == 0) {
        std::cout << "connection close" << std::endl;
        return ret;
    } else if (ret < 0) {
        std::cout << "read error" << std::endl;
        return ret;
    } else {
        send(clientSock, buffer, ret, 0);
    }
    return ret;
}

int main()
{
    int maxSock = 0;
    TcpServer server(INADDR_ANY, 8000, false);
    fd_set readSet,  allSet;
    FD_ZERO(&allSet);
    FD_SET(server._serverSock, &allSet);
    maxSock = server._serverSock;
    while (1) {
        readSet = allSet;
        int rc = select(maxSock + 1, &readSet, NULL, NULL, NULL);
        if (rc <= 0) {
            std::cout << "select error" << std::endl;
            continue;
        }
        for (int i = 0; i <= maxSock; i++) {
            if (i == server._serverSock) {
                continue;
            }
            if (FD_ISSET(i, &readSet)) {
                int ret = ReadAndSendBack(i);
                if (ret <= 0) {
                    FD_CLR(i, &allSet);
                }
            }
        }
        if (FD_ISSET(server._serverSock, &readSet)) {
            sockaddr clientAddr;
            int clientSock = server.Accept(clientAddr);
            if (clientSock < 0) {
                std::cout << "accept error" << std::endl;
            }
            FD_SET(clientSock, &allSet);
            maxSock = std::max(maxSock, clientSock);
        }
    }    
}