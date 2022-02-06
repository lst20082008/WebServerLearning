#include "netWorkUtil.h"
#include <netinet/ip.h>
#include <iostream>
#include "time.h"
#include <queue>

class BlockQueue {
private:
    std::queue<int> _q;
    int _capacity;
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
public:
    std::vector<double> _costTime;
    BlockQueue(int capacity)
    {
        _capacity = capacity + 1;
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
        if (ret != -1) {
            _q.pop();
        }
        pthread_mutex_unlock(&_mutex);
        return ret;
    }

    void AddCostTime(double cost)
    {
        pthread_mutex_lock(&_mutex);
        _costTime.push_back(cost);
        pthread_mutex_unlock(&_mutex);
    }

    void AddInitFd()
    {
        pthread_mutex_lock(&_mutex);
        for (int i = 0; i < _capacity - 1; i++) {
            _q.push(1);
        }
        pthread_cond_broadcast(&_cond);
        pthread_mutex_unlock(&_mutex);
    }
};

void TestCase()
{
    TcpClient client(false);
    client.Connect(INADDR_LOOPBACK, 8000);
    std::string total, msg;
    client.Send("hello1");
    client.Send("hello2");
    client.Send("hello3");
    while (total.size() < 18) {
        int ret = client.Read(msg);
        if (ret > 0) {
            total += msg;
        } else {
            std::cerr << "[error]read failed!" << std::endl;
            break;
        }
    }
    if (total != std::string("hello1hello2hello3")) {
        std::cerr << "test failed" << std::endl;
    }
    
}

void *threadRun(void *arg)
{
    BlockQueue* bq = (BlockQueue*)arg;
    while (1) {
        int task = bq->GetFd();
        if (task == -1) {
            return nullptr;
        }
        clock_t start_t, end_t;
        start_t = clock();
        TestCase();
        end_t = clock();
        bq->AddCostTime((double)(end_t - start_t) / CLOCKS_PER_SEC);
    }
}

int main(int argc, char** argv)
{
    int totalCount, maxConcurrentCount;
    if (argc < 3) {
        std::cout << "please enter total test count and max concurrent count" << std::endl;
        return -1;
    }
    totalCount = atoi(*(argv + 1));
    maxConcurrentCount = atoi(*(argv + 2));
    std::cout << "[check]totalCount:" << totalCount << std::endl;
    std::cout << "[check]maxConcurrentCount:" << maxConcurrentCount << std::endl;
    
    BlockQueue blockQueue(totalCount);
    std::vector<pthread_t> threadPool;
    std::cout << "initailizing thread pool...";
    for (int i = 0; i < maxConcurrentCount; i++) {
        pthread_t pid;
        pthread_create(&pid, NULL, threadRun, &blockQueue);
        threadPool.push_back(pid);
    }
    std::cout << "done." << std::endl;

    std::cout << "adding tasks...";
    blockQueue.AddInitFd();
    std::cout << "done." << std::endl;
    blockQueue.AddFd(-1);

    std::cout << "wait thread join..." << std::endl;
    for (const auto& pid:threadPool) {
        pthread_join(pid, NULL);
        std::cout << "thread:" << pid << "join" << std::endl;
    }

    double totalTime, maxDelay, averageTime;
    for (const double& cost : blockQueue._costTime) {
        totalTime += cost;
        maxDelay = cost > maxDelay ? cost : maxDelay;
    }
    averageTime = totalTime / blockQueue._costTime.size();
    std::cout << "[result]总体用时:" << totalTime << std::endl;
    std::cout << "[result]最大单次用时:" << maxDelay << std::endl;
    std::cout << "[result]平均用时:" << averageTime << std::endl;
    return 0;
}