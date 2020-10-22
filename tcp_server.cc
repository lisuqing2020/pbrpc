#include "tcp.h"

TcpServer::TcpServer() {
    monitor_ = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(monitor_, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
}

void TcpServer::Listen(unsigned short port) {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(monitor_, (struct sockaddr*)&addr, sizeof(addr));
    listen(monitor_, 128);
}

TcpSocket* TcpServer::Accept(int& r, int timeout) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(monitor_, &readfds);
    struct timeval tv = {timeout, 0};
    int ret = select(monitor_+1, &readfds, NULL, NULL, &tv);
    if(ret == 0) {
        // 超时...写缓冲区还是满的
        r = -1;
    } else if(ret > 0) {
        // 可读...
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        int socket = accept(monitor_, (struct sockaddr*)&addr, &len);
        r = 0;
        return new TcpSocket(socket);
    } else {
        cout << "select error\n";
        // select出错...
    }
    return nullptr;
}

TcpServer::~TcpServer() {
    close(monitor_);
}
