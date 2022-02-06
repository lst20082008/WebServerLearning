#include "netWorkUtil.h"
#include <netinet/ip.h>
#include <iostream>

int main()
{
    TcpClient client(false);
    client.Connect(INADDR_LOOPBACK, 8000);
    while (1) {
        std::string msg;
        std::cin >> msg;
        client.Send(msg);
        int ret = client.Read(msg);
        if (ret > 0) {
            std::cout << msg << std::endl;
        } else if (ret == 0) {
            std::cout << "connection close!" << std::endl;
            break;
        } else {
            std::cout << "error!" << std::endl;
            break;
        }
    }
}