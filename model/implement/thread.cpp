/*#include "../heaters/thread.h"

Thread::Thread() : running(false) {}
Thread::~Thread() {
    stop();
    if (t.joinable()) {
        t.join();
    }
}
void Thread::execute(std::function<void()> f) {
    if (running.load()) {
        return;
    }
    func = f;
    running.store(true);
    t = std::thread([this]() {
        while (running.load()) {
            func();
        }
    });
}
void Thread::stop() {
    running.store(false);
}

bool Thread::isRunning() const {
    return running.load();
}
#endif // THREAD_H
*/