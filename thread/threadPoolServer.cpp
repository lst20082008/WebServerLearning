#include "netWorkUtil.h"
#include <iostream>
#include "pthread.h"
#include <queue>

class BlockQueue {
private:
    std::queue<int> _q;
    int _capacity;
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
public:
    BlockQueue(int capacity)
    {
        _capacity = capacity;
        pthread_mutex_init(&_mutex, NULL);
        pthread_cond_init(&_cond, NULL);
    }

    int AddFd(int fd)
    {
        pthread_mutex_lock(&_mutex);
        if (_q.size() >= _capacity) {
            std::cout << "BlockQueue full!" << std::endl;
            return -1;
        }
        _q.push(fd);
        std::cout << "add fd:" << fd << std::endl;
        pthread_cond_signal(&_cond);
        pthread_mutex_unlock(&_mutex);
        return 0;
    }

    int GetFd()
    {
        pthread_mutex_lock(&_mutex);
        while (_q.size() == 0) {
            pthread_cond_wait(&_cond, &_mutex);
        }
        int ret = _q.front();
        _q.pop();
        std::cout << "get fd:" << ret << std::endl;
        pthread_mutex_unlock(&_mutex);
        return ret;
    }
};


void ProcessData(int clientSock)
{
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

void *thread_run(void *arg)
{
    pthread_detach(pthread_self());
    BlockQueue* bq = (BlockQueue*)arg;
    while (1) {
        int clientSock = bq->GetFd();
        if (clientSock > 0) {
            ProcessData(clientSock);
        }
    }
    
}

int main()
{
    TcpServer server(INADDR_ANY, 8000, false);
    BlockQueue bq(5);
    for (int i = 0; i < 5; i++) {
        pthread_t pthreadId;
        pthread_create(&pthreadId, NULL, thread_run, &bq);
    }
    while (1)
    {
        sockaddr clientAddr;
        int clientSock = server.Accept(clientAddr);
        if (clientSock < 0) {
            std::cout << "accept error" << std::endl;
            continue;
        }
        bq.AddFd(clientSock);
    }
}