#ifndef _TCP_H_
#define _TCP_H_

#include <string>
#include <arpa/inet.h> 
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
using namespace std;

class TcpSocket {
private:
    int socket_;
public:
    TcpSocket();
    TcpSocket(int socket);

    void Connect(string ip, unsigned short port, int timeout);
    void Send(string data, int timeout);
    string Recv(int timeout);

    ~TcpSocket();
};

class TcpServer {
private:
    int monitor_;
public:
    TcpServer();

    void Listen(unsigned short port);
    TcpSocket* Accept(int& r, int timeout);

    ~TcpServer();
};

#endif