#ifndef THREADUTILS_H
#define THREADUTILS_H

#include <pthread.h>
#include <sched.h>
#include <sys/resource.h>
#include <thread>
#include <vector>

void setThreadPriority(std::thread &t, int priority);
void setThreadAffinity(std::thread &t, std::vector<int> cores);

#endif // THREADUTILS_H