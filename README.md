# netco

a network coroutine library based on c++ 11

to be better

1. in Socket.h , using smart pointer to manage sockfd life is a better choice
2. in spinlock, using atomic_flag can avoid lockoing expense

tips ：

1. 为了更好地管理锁的生命周期并确保锁的正确释放，所以定义 class spinLockGuard 来自动管理锁的获取和释放
