#include "tcp.h"

TcpSocket::TcpSocket() {
    socket_ = socket(AF_INET, SOCK_STREAM, 0);
}

TcpSocket::TcpSocket(int socket) {
    socket_ = socket;
}

void TcpSocket::Connect(string ip, unsigned short port, int timeout) {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr.s_addr);

    int flag = fcntl(socket_, F_GETFL);
    flag = flag | O_NONBLOCK;
    fcntl(socket_, F_SETFL, flag);

    int ret = connect(socket_, (struct sockaddr*)&addr, sizeof(addr));
    if(ret == -1 && errno == EINPROGRESS) {
        // POSIX定义了select/epoll和非阻塞connect的相关规定：
        // 连接成功时：socket变为可写。
        // 连接失败时：socket变为可读可写。
        fd_set writefds;
        FD_ZERO(&writefds);
        FD_SET(socket_, &writefds);
        struct timeval tv = {timeout, 0};
        int ret = select(socket_+1, NULL, &writefds, NULL, &tv);
        if(ret == 0) {
            // 超时...
        } else if(ret > 0) {
            int connect_status;
            socklen_t len = sizeof(connect_status);
            getsockopt(socket_, SOL_SOCKET, SO_ERROR, &connect_status, &len);
            if(connect_status == 0) {
                // 连接成功...
            } else {
                // 连接失败...
            }
        } else {
            // select出错...
        }
    }
    flag &= ~O_NONBLOCK;
    fcntl(socket_, F_SETFL, flag);
}

void TcpSocket::Send(string data, int timeout) {
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(socket_, &writefds);
    struct timeval tv = {timeout, 0};
    int ret = select(socket_+1, NULL, &writefds, NULL, &tv);
    if(ret == 0) {
        // 超时...写缓冲区还是满的
    } else if(ret > 0) {
        // 写缓冲区有空闲，但不保证够用
        write(socket_, data.c_str(), data.size()+1);
    } else {
        // select出错...
    }
}

string TcpSocket::Recv(int timeout) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(socket_, &readfds);
    struct timeval tv = {timeout, 0};
    int ret = select(socket_+1, NULL, &readfds, NULL, &tv);
    if(ret == 0) {
        // 超时...写缓冲区还是满的
    } else if(ret > 0) {
        // 可读...
        char buf[4096];
        read(socket_, buf, sizeof(buf));
        return string(buf);
    } else {
        // select出错...
        cout << "select error\n";
    }
}

TcpSocket::~TcpSocket() {
    close(socket_);
}