#include <easygo/easygo.h>
#include <easygo/Socket.h>
#include <easygo/Mutex.h>

void single_acceptor_server_test()
{
    easygo::co_go(
        []{
            easygo::Socket listener;
            if (listener.isUseful())
            {
                listener.setTcpNoDelay(true);

                listener.setReuseAddr(true);

                listener.setReusePort(true);

                if (listener.bind(8099) < 0)
                {
                    std::cout << "bind error" << std::endl;
                    return ;
                }
                listener.listen();
            }
            while (true)
            {
                easygo::Socket* conn = new easygo::Socket(listener.accept());

                conn->setTcpNoDelay(true);

                easygo::co_go(
                    [conn]
                    {
                        std::string hello("HTTP/1.0 200 OK\r\nServer: easygo/0.1.0\r\nContent-Length: 13\r\n");
                        char buf[1024];
                        if (conn->read(buf, sizeof(buf)) > 0 )
                        {
                            conn->send(hello.c_str(), hello.size());
                            easygo::co_wait(50);
                        }
                        delete conn;
                    }
                );
            }
        }
    );
}

void client_test()
{
    easygo::co_go(
        []
        {
            char buf[1024];
            while (true)
            {
                easygo::co_wait(2000);
                
                easygo::Socket s;

                s.connect("127.0.0.1", 8099);

                s.send("ping", 4);

                s.read(buf, sizeof(buf));

                std::cout << buf << std::endl;

            }
        }
    );
}

// 读写锁测试

void mutex_test(easygo::RWMutex& mu)
{
    for (int i = 0; i < 10; ++i)
    {
        if (i < 5)
        {
            easygo::co_go(
                [&mu, i]
                {
                    mu.RLock();

                    std::cout << i << " : start reading" << std::endl;

                    easygo::co_wait(100);

                    std::cout << i << " : finish reading" << std::endl;

                    mu.RUnlock();

                    mu.WLock();

                    std::cout << i << " : start writing" << std::endl;

                    easygo::co_wait(100);

                    std::cout << i << " : finish writing" << std::endl;

                    mu.WUnlock();
                }
            );
        }
        else
        {
            easygo::co_go(
                [&mu, i]
                {

                    mu.WLock();

                    std::cout << i << " : start writing" << std::endl;

                    easygo::co_wait(100);

                    std::cout << i << " : finish writing" << std::endl;

                    mu.WUnlock();
                    mu.RLock();

                    std::cout << i << " : start reading" << std::endl;

                    easygo::co_wait(100);

                    std::cout << i << " : finish reading" << std::endl;

                    mu.RUnlock();

                    
                }
            );
        }
    }
}


int main()
{
    std::cout << "start" << std::endl;


    easygo::RWMutex mu;

    mutex_test(mu);

    std::cout << "mutex test end" << std::endl;

    single_acceptor_server_test();

    std::cout << "single acceptor server test end" << std::endl;
    
    client_test();

    std::cout << "client test end" << std::endl;

    easygo::scheduler_join();

    std::cout << "end" << std::endl;

    return 0;
}