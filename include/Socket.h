#pragma once

#include "noncopyable.h"
#include "Logger.h"
#include "Parameter.h"

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

namespace netco
{
/* Socket class : nonblocking fd is default
 * intention : 
 * 1. provide API for sockfd operations
 * 2. manage life cycle of sockfd, by reference counting.
 */

    class Socket //: noncopyable
    {
    public:
        explicit Socket(int sockfd, std::string ip = "", int port = 0);
        Socket();
        ~Socket();
        const int fd() const { return _sockfd; }
        const std::string& ip() const { return _ip; }
        const int port() const { return _port; }
        // 返回当前socket是否可用
        bool isUseful() const { return _sockfd > 0; }

        //  绑定ip port 到 socket
        int bind(int port);

        // 监听当前socket
        int listen();

        // 返回连接端的socket
        Socket accept();

        ssize_t read(void* buf, size_t len);

        ssize_t send(const void* buf, size_t len ); 

        void connect(const char* ip, int port);
        // 获取套接字的选项,成功则返回true，反之，返回false
        bool getSocketOpt(struct tcp_info* info) const;

        bool getSocketOptString(char* buf, int len);

        std::string getSocketOptString() const;
        
        // 关闭socket的写操作
        int shutdownWrite();

        // 禁用Nagle算法以减少延迟,
        // 可以通过设置TCP_NODELAY选项来实现
        int setTcpNoDelay(bool on);

        int setReuseAddr(bool on);

        int setReusePort(bool on);

        int setKeepAlive(bool on);

        int setNonBlockSocket();;

        int setBlockSocket();

    private:

        //接收一个连接，返回一个新连接的Socket
		Socket accept_raw();

        const int _sockfd;
        // 引用计数
        int* _pRef;

        int _port;

        std::string _ip;

    };
}