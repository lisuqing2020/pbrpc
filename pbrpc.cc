#include <string>
#include <map>
#include <memory>
#include <thread>
#include <pthread.h>
#include <functional>
#include <chrono>
#include <any>
#include "tcp.h"

class RpcServer {
public:
    RpcServer(short port): port_(port) {
        tcp_server_.Listen(port_);
    }

    void Register(const std::string name, const std::any& f) {
        map_[name] = f;
    }

    void Run() {
        while(true) {
            int ret = 0;
            // auto sock = make_shared<TcpSocket>(*tcp_server_.Accept(ret, 3));
            TcpSocket* sock = tcp_server_.Accept(ret, 3);
            if(ret != -1) {
                std::thread thd(bind(Handler, sock));
                thd.join();
            }
        }
    }

private:
    static std::map<std::string, std::any> map_;
    short port_;
    TcpServer tcp_server_;

    static void Handler(TcpSocket* sock) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        cout << "handling..." << endl;
        // std::string name = sock->Recv(0);
        std::string name = "add";
        auto func = std::any_cast<function<int(int,int)>>(map_["add"]);
        int result = func(1,2);
        std::cout << result << std::endl;
    }
};

std::map<std::string, std::any> RpcServer::map_ = std::map<std::string, std::any>();

int add(int a, int b) {
    return a + b;
}

int main() {
    RpcServer server(9000);
    server.Register("add", function<int(int,int)>(add));
    server.Run();
    return 0;
}
