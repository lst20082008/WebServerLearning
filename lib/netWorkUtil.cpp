#include "netWorkUtil.h"

#include <string>

TcpServer::TcpServer(unsigned int ip, short port, bool blockio)
{
    _serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSock == -1) {
        std::cout << "create serversock failed" << std::endl;
    }

    if (blockio) {
        if(fcntl(_serverSock, F_SETFL, O_NONBLOCK) == -1) {
            std::cout << "set non-block failed" << std::endl;
        }
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = htonl(ip);
    if (bind(_serverSock, (sockaddr*)&serverAddress, sizeof(sockaddr_in)) < 0) {
        std::cout << "bind error" << std::endl;
    }

    if (listen(_serverSock, 5) < 0) {
        std::cout << "listen error" << std::endl;
    }
}

TcpServer::~TcpServer()
{
    close(_serverSock);
}

int TcpServer::Accept(sockaddr& clientAddr)
{
    socklen_t socklen = sizeof(sockaddr_in);
    int clientSock = accept(_serverSock, &clientAddr, &socklen);
    if (clientSock < 0) {
        std::cout << "accept error" << std::endl;
    }
    return clientSock;
}

TcpClient::TcpClient(bool blockio)
{
    _isConnected = false;
    _clientSock = socket(AF_INET, SOCK_STREAM, 0);
    if (_clientSock == -1) {
        std::cout << "create clientSock failed" << std::endl;
    }

    if (blockio) {
        if(fcntl(_clientSock, F_SETFL, O_NONBLOCK) == -1) {
            std::cout << "set non-block failed" << std::endl;
        }
    }
}

TcpClient::~TcpClient()
{
    close(_clientSock);
}

int TcpClient::Connect(unsigned int ip, short port)
{
    int ret;
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = htonl(ip);
    if (ret = connect(_clientSock, (sockaddr*)&serverAddress, sizeof(sockaddr_in)) < 0) {
        std::cout << "connect error" << std::endl;
    } else {
        _isConnected = true;
    }
    return ret;
}

int TcpClient::Send(const std::string& msg)
{
    int ret;
    if (!_isConnected) {
        std::cout << "not connected!" << std::endl;
        return -1;
    }
    if (ret = send(_clientSock, msg.c_str(), msg.size(), 0) == -1) {
        std::cout << "send error!" << std::endl;
    }
    return ret;
}

int TcpClient::Read(std::string& msg)
{
    char buffer[1024];
    if (!_isConnected) {
        std::cout << "not connected!" << std::endl;
        return -1;
    }
    int ret = read(_clientSock, buffer, 1024);
    if (ret == 0) {
        std::cout << "connection close" << std::endl;
        _isConnected = false;
    } else if (ret < 0) {
        std::cout << "read error" << std::endl;
    } else {
        msg = buffer;
        msg.resize(ret);
    }
    return ret;
}
