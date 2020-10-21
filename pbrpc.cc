#include <string>
#include <map>
#include <memory>
#include <thread>
#include <pthread.h>
#include <functional>
#include <chrono>
#include "tcp.h"

class RpcServer {
public:
    RpcServer(short port): port_(port) {
        tcp_server_.Listen(port_);
    }

    void Register(const std::string name, const function<int(int, int)>& f) {
        map_[name] = f;
    }

    void Run() {
        auto f = map_["add"];
        std::cout << f(1,2) << std::endl;
        while(true) {
            int ret = 0;
            auto sock = make_shared<TcpSocket>(*tcp_server_.Accept(ret, 3));
            if(ret != -1) {
                std::thread thd(Handler_);
                thd.join();
            }
        }
    }

private:
    std::map<std::string, function<int(int,int)>> map_;
    short port_;
    TcpServer tcp_server_;
    
    static void Handler_() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        cout << "handling..." << endl;
    }
};

int add(int a, int b) {
    return a + b;
}

int main() {
    RpcServer server(9000);
    server.Register("add", add);
    server.Run();
    return 0;
}