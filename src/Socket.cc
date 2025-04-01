#include "../include/Socket.h"

#include <errno.h>

using namespace netco;

Socket::Socket(int sockfd, std::string ip, int port)
    : _sockfd(sockfd)
    , _ip(ip)
    , _port(port)
{
    if (sockfd > 0)
    {
        setNonBlockSocket();
    }
    else
    {
        LOG_ERROR("%s:%s:%d create Socket Error, sockfd = %d\n",
                  __FILE__, __func__, __LINE__, sockfd);
    }
}
Socket::Socket()
    : _sockfd(::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))
    , _pRef(new int(1))
    , _port(-1)
    , _ip("")
{
}

Socket::~Socket()
{
    --(*_pRef);
    if (!(*_pRef) && isUseful())
    {
        ::close(_sockfd);
        delete _pRef;
    }
}


//  绑定ip port 到 socket
int Socket::bind(int port)
{
    _port = port;
    sockaddr_in serv = {0};
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(_port);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    int ret = ::bind(_sockfd, (sockaddr*)&serv, sizeof(serv));
    if (ret < 0)
    {
        LOG_FATAL("%s:%s:%d bind Socket Error, errno = %d\n",
        __FILE__, __func__, __LINE__, errno);
    }
    return ret;

}

// 监听当前socket
int Socket::listen()
{
    int ret = ::listen(_sockfd, Parameter::listenBacklog);
    if (ret < 0)
    {
        LOG_FATAL("%s:%s:%d listen Socket Error, errno = %d\n",
        __FILE__, __func__, __LINE__, errno);
    }
    return ret;
}

// 返回连接端的socket
Socket Socket::accept_raw()
{
    sockaddr_in client;
    socklen_t clientLen =sizeof(client);
    int connfd = ::accept(_sockfd, (sockaddr*)&client, &clientLen);
    if (connfd < 0)
    {
        LOG_ERROR("%s:%s:%d accept Socket Error, errno = %d\n",
        __FILE__, __func__, __LINE__, errno);
        return Socket(connfd);
    }
    sockaddr_in* sock = (sockaddr_in*)&client;
    int port = ntohs(sock->sin_port);
    char ip[64];
    inet_ntop(AF_INET, &sock->sin_addr, ip, sizeof(ip));
    return Socket(connfd, std::string(ip), port);
    

}

Socket Socket::accept()
{
    auto retSocket = accept_raw();
    if (retSocket.isUseful())
    {
        return retSocket;
    }
    Scheduler::getScheduler()->getProcessor(threadIdx)->waitEvent(_sockfd, EPOLLIN | EPOLLPRI | EPOLLRDHUP | EPOLLHUP);
	auto con(accept_raw());
	if(con.isUseful()){
		return con;
	}
	return accept();
}

ssize_t Socket::read(void* buf, size_t len)
{
    auto ret = ::read(_sockfd, buf, len);
    if (ret >= 0)   
    {
        return ret;
    }
    if (ret == -1 && errno == EINTR)
    {
        return read(buf, len);
    }
    Scheduler::getScheduler()->getProcessor(threadIdx)->waitEvent(_sockfd, EPOLLIN | EPOLLPRI | EPOLLRDHUP | EPOLLHUP);
	return ::read(_sockfd, buf, len);
}

ssize_t Socket::send(const void* buf, size_t len )
{
    // ignore SIGPIPE signal
    size_t sendIdx = ::send(_sockfd, buf, len, MSG_NOSIGNAL);
    if (sendIdx >= len)
    {
        return len;
    }
    Scheduler::getScheduler()->getProcessor(threadIdx)->waitEvent(_sockfd, EPOLLOUT);
	return send(static_cast<const char*>(buf) + sendIdx, len - sendIdx);

}

void Socket::connect(const char* ip, int port)
{
    sockaddr_in addr = {0};
    addr.sin_family= AF_INET;
	addr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &addr.sin_addr);
	_ip = std::string(ip);
	_port = port;
	auto ret = ::connect(_sockfd, (struct sockaddr*)&addr, sizeof(sockaddr_in));
	if(ret == 0){
		return;
	}
	if(ret == -1 && errno == EINTR){
		return connect(ip, port);
	}
	Scheduler::getScheduler()->getProcessor(threadIdx)->waitEvent(_sockfd, EPOLLOUT);
	 return connect(ip, port);

}

// 关闭socket的写操作
int Socket::shutdownWrite()
{
    int ret = ::shutdown(_sockfd, SHUT_WR);
    return ret;
}

// 获取套接字的选项,成功则返回true，反之，返回false
bool Socket::getSocketOpt(struct tcp_info* tcpi) const
{
    socklen_t len = sizeof(*tcpi);
    memset(tcpi, 0, sizeof(*tcpi));
    return ::getsockopt(_sockfd, SOL_TCP, TCP_INFO, tcpi, &len);

}

bool Socket::getSocketOptString(char* buf, int len) const
{
    tcp_info tcpi;
    bool ok = getSocketOpt(&tcpi);
    if (ok)
    {
        snprintf(buf, len, "unrecovered=%u "
			"rto=%u ato=%u snd_mss=%u rcv_mss=%u "
			"lost=%u retrans=%u rtt=%u rttvar=%u "
			"sshthresh=%u cwnd=%u total_retrans=%u",
			tcpi.tcpi_retransmits,  // Number of unrecovered [RTO] timeouts
			tcpi.tcpi_rto,          // Retransmit timeout in usec
			tcpi.tcpi_ato,          // Predicted tick of soft clock in usec
			tcpi.tcpi_snd_mss,
			tcpi.tcpi_rcv_mss,
			tcpi.tcpi_lost,         // Lost packets
			tcpi.tcpi_retrans,      // Retransmitted packets out
			tcpi.tcpi_rtt,          // Smoothed round trip time in usec
			tcpi.tcpi_rttvar,       // Medium deviation
			tcpi.tcpi_snd_ssthresh,
			tcpi.tcpi_snd_cwnd,
			tcpi.tcpi_total_retrans);  // Total retransmits for entire connection
    }
    return ok;
}

std::string Socket::getSocketOptString() const
{
    char buf[1024];
    buf[0] = '\0';
    getSocketOptString(buf, sizeof(buf));
    return std::string(buf);

}



// 禁用Nagle算法以减少延迟,
// 可以通过设置TCP_NODELAY选项来实现
int Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(_sockfd, IPPROTO_TCP, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof optval) );
    return ret;

}

int Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof optval));
    return ret;

}

int Socket::setReusePort(bool on)
{
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof optval));
    return ret;
}

int Socket::setKeepAlive(bool on)
{
    int optval  = on ? 1 : 0;
    int ret = ::setsockopt(_sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t>(sizeof optval));
    return ret;
}

int Socket::setNonBlockSocket()
{
    int flags = ::fcntl(_sockfd, F_GETFL, 0);
    int ret = ::fcntl(_sockfd, F_SETFL, flags | O_NONBLOCK);
    return ret;
}

int Socket::setBlockSocket()
{
    int flags = ::fcntl(_sockfd, F_GETFL, 0);
    int ret = ::fcntl(_sockfd, F_SETFL, flags & (~O_NONBLOCK));
    return ret;
}
