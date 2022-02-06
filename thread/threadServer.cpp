#include "netWorkUtil.h"
#include <iostream>
#include "pthread.h"

void *thread_run(void *arg)
{
    pthread_detach(pthread_self());
    int clientSock = *(int*)arg;
    // child
    char buffer[1024];
    while(1) {
        int ret = read(clientSock, buffer, 1024);
        if (ret == 0) {
            std::cout << "connection close" << std::endl;
            close(clientSock);
            break;
        } else if (ret < 0) {
            std::cout << "read error" << std::endl;
            break;
        } else {
            send(clientSock, buffer, ret, 0);
        }
    }
}

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
        pthread_t pthreadId;
        pthread_create(&pthreadId, NULL, thread_run, &clientSock);
    }
}