/*#ifndef THREAD_H
#define THREAD_H

#include <thread>
#include <functional>
#include <atomic>

class Thread {
private:
    std::thread t;
    std::function<void()> func;
    std::atomic<bool> running;

public:
    Thread();
    ~Thread();

    void execute(std::function<void()> f);
    void stop();
    bool isRunning() const;
};

#endif // THREAD_H
*/