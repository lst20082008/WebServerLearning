#ifndef __NETWORK_UTIL_H
#define __NETWORK_UTIL_H
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
class TcpServer {
public:
    int _serverSock;
    TcpServer(unsigned int ip, short port, bool blockio);
    ~TcpServer();
    int Accept(sockaddr& clientAddr);
};
class TcpClient {
private:
    int _clientSock;
    bool _isConnected;
public:
    TcpClient(bool blockio);
    ~TcpClient();
    int Connect(unsigned int ip, short port);
    int Send(const std::string&);
    int Read(std::string& msg);
};
#endif